#include "bencode.h"
#include "utils.h"
#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum bencode_type
get_bencode_type (const char *bencoded_value)
{
  if (*bencoded_value == 'i')
    return BENCODE_INTEGER;
  else if (is_digit (*bencoded_value))
    return BENCODE_STRING;
  else if (*bencoded_value == 'l')
    return BENCODE_LIST;
  else if (*bencoded_value == 'd')
    return BENCODE_DICTIONARY;

  return BENCODE_INVALID;
}

/** stdarg in future but now not required **/
bencode_dictionary *
get_dictionary_from_key (bencode *decode, const char *key)
{
  switch (decode->type)
    {
    case BENCODE_DICTIONARY:
      {
        int length = 0;
        bencode_dictionary *dict = (bencode_dictionary *)decode;
        while (length < dict->length)
          {
            if (strcmp (dict->keys[length]->value, key) == 0)
              {
                return (bencode_dictionary *)dict->values[length];
              }
            length++;
          }
        return NULL;
        break;
      }
    case BENCODE_LIST:
      {
        int length = 0;
        bencode_list *lst = (bencode_list *)decode;
        while (length < lst->length)
          {
            if (lst->values[length]->type == BENCODE_DICTIONARY)
              {
                bencode_dictionary *bdict
                    = (bencode_dictionary *)get_dictionary_from_key (
                        lst->values[length], key);
                if (bdict != NULL)
                  return bdict;
              }
          }
      }
    default:
      break;
    }
  return NULL;
}
char *
encode_dict_bencode (bencode *bencoded_dict)
{
  bencode_dictionary *bdict = (bencode_dictionary *)bencoded_dict;
  char *buf = (char *)malloc (sizeof (char) * bdict->raw_size);
  memset (buf, '\0', bdict->raw_size);
  int str_index = 0, temp = 0;
  buf[str_index] = 'd';
  str_index++;
  while (temp < bdict->length)
    {
      bencode_string *key = bdict->keys[temp];
      snprintf (buf + str_index, key->raw_size + 1, "%zu:%s",
                strlen (key->value), key->value);
      str_index += key->raw_size;

      bencode *value = bdict->values[temp];
      switch (bdict->values[temp]->type)
        {
        case BENCODE_STRING:
          {
            bencode_string *val = (bencode_string *)bdict->values[temp];
            snprintf (buf + str_index, val->raw_size + 1, "%zu:%s",
                      strlen (val->value), val->value);
            str_index += val->raw_size;
            break;
          }
        case BENCODE_INTEGER:
          {
            bencode_integer *val = (bencode_integer *)bdict->values[temp];
            snprintf (buf + str_index, val->raw_size + 1, "i%lde", val->value);
            str_index += val->raw_size;
            break;
          }
        case BENCODE_LIST:
          {
            char *list_element = encode_list_bencode (bdict->values[temp]);
            memcpy (buf + str_index, list_element,
                    bdict->values[temp]->raw_size);
            str_index += bdict->values[temp]->raw_size;
            free (list_element);
            break;
          }
        case BENCODE_DICTIONARY:
          {
            char *dict_element = encode_dict_bencode (bdict->values[temp]);
            memcpy (buf + str_index, dict_element,
                    bdict->values[temp]->raw_size);
            str_index += bdict->values[temp]->raw_size;
            free (dict_element);
            break;
          }
        case BENCODE_INVALID:
          {
            fprintf (stderr, "bencode invalid type");
            break;
          }
        }
      temp++;
    }
  buf[str_index] = 'e';
  return buf;
}
char *
encode_list_bencode (bencode *lst)
{
  bencode_list *blst = (bencode_list *)lst;
  int length = 0, str_index = 0;
  char *arr = (char *)malloc (sizeof (char) * lst->raw_size);
  memset (arr, '\0', sizeof (char) * lst->raw_size);
  arr[str_index] = 'l';
  while (length < blst->length)
    {
      switch (blst->values[length]->type)
        {
        case BENCODE_STRING:
          {
            bencode_string *val = (bencode_string *)blst->values[length];
            snprintf (arr + str_index, val->raw_size, "%zu:%s",
                      strlen (val->value), val->value);
            str_index += val->raw_size;
            break;
          }
        case BENCODE_INTEGER:
          {
            bencode_integer *val = (bencode_integer *)blst->values[length];
            snprintf (arr + str_index, val->raw_size, "i%lde", val->value);
            str_index += val->raw_size;
            break;
          }
        case BENCODE_LIST:
          {
            char *lst_element = encode_list_bencode (blst->values[length]);
            memcpy (arr + str_index, lst_element,
                    blst->values[length]->raw_size);
            str_index += blst->values[length]->raw_size;
            free (lst_element);
            break;
          }
        case BENCODE_DICTIONARY:
          {
            char *dict_element = encode_dict_bencode (blst->values[length]);
            memcpy (arr + str_index, dict_element,
                    blst->values[length]->raw_size);
            str_index += blst->values[length]->raw_size;
            break;
          }
        case BENCODE_INVALID:
          {
            fprintf (stderr, "invalid bencode type\n");
            break;
          }
        }
    }
  arr[blst->raw_size - 1] = 'e';
  return arr;
}

bencode *
decode_dict_bencode (const char *bencoded_value)
{
  /* d3:fooi53e5:hello:i34343e*/
  printf ("[your program] decode_dict is started\n");
  const char *curr = ++bencoded_value;
  bencode_dictionary *bdict = (bencode_dictionary *)malloc (sizeof (*bdict));
  bdict->type = BENCODE_DICTIONARY;
  bdict->raw_size = 2;
  bdict->length = 0;
  bdict->keys = NULL;
  bdict->values = NULL;
  int local_element_counter = 0;
  while (curr - bencoded_value < strlen (bencoded_value) - 1)
    {
      if (*curr == 'e')
        return (bencode *)bdict;

      bencode_string *b_key = (bencode_string *)decode_bencode (curr);
      printf ("[your_program] dict_key value is %s\n", b_key->value);
      /* in the end error after that log and it's not assert, maybe its a
       * realloc func, but how */
      bdict->keys = realloc (bdict->keys, bdict->length + 1);
      assert (bdict->keys != NULL);
      bdict->keys[bdict->length] = b_key;
      curr += b_key->raw_size;

      bencode *b_value = decode_bencode (curr);
      printf ("[your program] dict value type is %d\n", b_value->type);
      bdict->values = realloc (bdict->values, bdict->length);
      assert (bdict->values != NULL);
      bdict->values[bdict->length] = b_value;
      curr += b_value->raw_size;

      bdict->raw_size += b_key->raw_size + b_value->raw_size;
      bdict->length++;
    }

  return (bencode *)bdict;
}

bencode *
decode_list_bencode (const char *bencoded_value)
{
  printf ("[your program] decode_list is started\n");
  char *curr;
  bencode_list *blist = (bencode_list *)malloc (sizeof (bencode_list));
  blist->type = BENCODE_LIST;
  blist->raw_size = 2;
  blist->length = 0;
  blist->values = NULL;
  curr = (char *)bencoded_value + 1;
  while (curr - bencoded_value < strlen (bencoded_value) - 1)
    {
      if (curr[0] == 'e')
        return (bencode *)blist;
      bencode *b = decode_bencode (curr);
      blist->length++;
      blist->values = realloc (blist->values, b->raw_size);
      if (blist->values == NULL)
        {
          fprintf (stderr, "realloc error\n");
          exit (0);
        }
      blist->values[blist->length - 1] = b;
      curr += b->raw_size;
      blist->raw_size += b->raw_size;
    }
  return (bencode *)blist;
}
bencode *
decode_integer_bencode (const char *bencoded_value)
{
  printf ("[your program] decode_integer is started\n");
  bencode_integer *bint = (bencode_integer *)malloc (sizeof (bencode_integer));
  bint->type = BENCODE_INTEGER;
  char *end = strchr (bencoded_value, 'e');
  char *is_negative = strchr (bencoded_value, '-');
  if (!end)
    {
      printf ("If you want to decode integer you need to write \'e\' in "
              "the end\n");
      exit (1);
    }
  int digit_length = end - bencoded_value;
  *end = '\0';
  if (!is_negative)
    {

      if (!is_str_digit_compatible (&bencoded_value[1]))
        {
          fprintf (stderr, "Between i and e you must use only numbers\n");
          exit (1);
        }
    }
  else
    {
      if (!is_str_digit_compatible (&bencoded_value[2]))
        {
          fprintf (stderr, "Between i and e you must use only numbers\n");
          exit (1);
        }
    }
  if (!is_negative ? (bencoded_value[1] == '0' && bencoded_value[2] != 'e')
                   : (bencoded_value[2] == '0' && bencoded_value[3] != 'e'))
    {
      fprintf (stderr, "Invalid, you can't use form like i02e and also i-02e "
                       "for example, write "
                       "your number without zero\n");
      exit (1);
    }

  if (is_str_number_overflow (&bencoded_value[1]))
    {
      fprintf (stderr, "Your int value are overflow\n");
      exit (1);
    }
  /** OLD RELEASE **/
  long int *decode_number = (long int *)malloc (sizeof (long long int));
  *decode_number = strtol (&bencoded_value[1], NULL, 10);
  bint->value = *decode_number;
  bint->raw_size = end - bencoded_value + 1;
  *end = 'e';
  return (bencode *)bint;
}
bencode *
decode_string_bencode (const char *bencoded_value)
{
  printf ("[your program] decode_string is started\n");
  bencode_string *str = (bencode_string *)malloc (sizeof (bencode_string));
  str->type = BENCODE_STRING;

  char *colon_index = strchr (bencoded_value, ':');
  if (colon_index != NULL)
    {
      *colon_index = '\0';
      int str_size = atoi (bencoded_value);
      *colon_index = ':';
      int length_before_colon = colon_index - bencoded_value;
      const char *start = colon_index + 1;
      char *decoded_str = (char *)malloc (str_size + 1);
      strncpy (decoded_str, start, str_size);
      decoded_str[str_size] = '\0';
      str->value = decoded_str;
      str->raw_size = length_before_colon + 1 + str_size;
      printf ("[your program] str is %s\n", str->value);
      return (bencode *)str;
    }
  else
    {
      fprintf (stderr, "Unhandled error in decode_string bencode - %s\n",
               bencoded_value);
      exit (0);
    }
}

void
bencode_json (bencode *b)
{
  switch (b->type)
    {
    case BENCODE_STRING:
      {
        printf ("\"%s\"", ((bencode_string *)b)->value);
        break;
      }
    case BENCODE_INTEGER:
      {
        printf ("%ld", ((bencode_integer *)b)->value);
        break;
      }
    case BENCODE_LIST:
      {
        int length = 0;
        bencode_list *list = ((bencode_list *)b);
        printf ("[");
        while (length < list->length)
          {
            bencode_json (list->values[length]);
            if (length < list->length - 1)
              {
                printf (",");
              }
            length++;
          }
        printf ("]");
        break;
      }
    case BENCODE_DICTIONARY:
      {
        bencode_dictionary *dict = (bencode_dictionary *)b;
        int length = 0;
        printf ("{");
        while (length < dict->length)
          {
            bencode_json ((bencode *)dict->keys[length]);
            printf (":");
            bencode_json (dict->values[length]);
            if (length < dict->length - 1)
              printf (",");
            length++;
          }
        printf ("}");
        break;
      }
    default:
      {
        printf ("unsupported\n");
        break;
      }
    }
}
bencode *
decode_bencode (const char *bencoded_value)
{
  enum bencode_type type = get_bencode_type (bencoded_value);
  printf ("[your program] type is returned its a %d\n", type);
  switch (type)
    {
    case BENCODE_STRING:
      return decode_string_bencode (bencoded_value);
    case BENCODE_INTEGER:
      return decode_integer_bencode (bencoded_value);
    case BENCODE_LIST:
      return decode_list_bencode (bencoded_value);
    case BENCODE_DICTIONARY:
      return decode_dict_bencode (bencoded_value);
    case BENCODE_INVALID:
      fprintf (stderr, "Invalid encoded value: %s\n", bencoded_value);
      exit (1);
    }

  return NULL;
}

void
bencode_print_dict_from_key (bencode *b, int key_sum, const char *key, ...)
{
  va_list args;
  bencode_dictionary *bdict = (bencode_dictionary *)b;
  int counter = 0, before_colon_sz;
  const char *next_arg;
  va_start (args, key);

  while (counter < bdict->length)
    {
      if (strcmp (bdict->keys[counter]->value, key) == 0)
        {
          const char *next_arg = va_arg (args, const char *);
          if (key_sum > 1)
            {
              bencode_print_dict_from_key (bdict->values[counter], --key_sum,
                                           next_arg, args);
              break;
            }
          printf ("%s: ", bdict->keys[counter]->value);
          switch (bdict->values[counter]->type)
            {
            case BENCODE_STRING:
              {
                printf ("%s",
                        ((bencode_string *)bdict->values[counter])->value);
                break;
              }
            case BENCODE_INTEGER:
              {
                bencode_json (bdict->values[counter]);
                break;
              }
            case BENCODE_LIST:
              {
                bencode_json (bdict->values[counter]);
                break;
              }
            case BENCODE_DICTIONARY:
              {
                bencode_json (bdict->values[counter]);
                break;
              }
            case BENCODE_INVALID:
              {
                break;
              }
            }
          printf ("\n");
          break;
        }
      counter++;
    }
  va_end (args);
}
void
bencode_free (bencode *b)
{
  switch (b->type)
    {
    case BENCODE_STRING:
      {
        free (((bencode_string *)b)->value);
        break;
      }
    case BENCODE_INTEGER:
      {
        break;
      }
    case BENCODE_LIST:
      {
        bencode_list *blist = (bencode_list *)b;
        int length = 0;
        if (blist->length == 0)
          {
            break;
          }
        while (length < blist->length)
          {
            bencode_free (blist->values[length]);
            length++;
          }
        break;
      }
    case BENCODE_DICTIONARY:
      {
        bencode_dictionary *bdict = (bencode_dictionary *)b;
        int length = 0;
        if (bdict->length == 0)
          {
            break;
          }
        while (length < bdict->length)
          {
            bencode_free ((bencode *)bdict->keys[length]);
            bencode_free ((bencode *)bdict->values[length]);
            length++;
          }
        break;
      }
    case BENCODE_INVALID:
      break;
    }
}
