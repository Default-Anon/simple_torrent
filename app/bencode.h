#ifndef __BENCODE_H_
#define __BENCODE_H_

enum bencode_type
{
  BENCODE_INVALID,
  BENCODE_INTEGER,
  BENCODE_STRING,
  BENCODE_LIST,
  BENCODE_DICTIONARY
};

typedef struct
{
  enum bencode_type type;
  int raw_size;
} bencode;

typedef struct
{
  enum bencode_type type;
  int raw_size;
  char *value;
} bencode_string;

typedef struct
{
  enum bencode_type type;
  int raw_size;
  long int value;
} bencode_integer;

typedef struct
{
  enum bencode_type type;
  int raw_size;
  int length;
  bencode **values;
} bencode_list;

typedef struct
{
  enum bencode_type type;
  int raw_size;
  int length;
  bencode_string **keys;
  bencode **values;
} bencode_dictionary;

bencode *bencode_invalid ();
bencode *decode_string_bencode (const char *bencoded_value);
bencode *decode_integer_bencode (const char *bencoded_value);
bencode *decode_list_bencode (const char *bencoded_value);
bencode *decode_dict_bencode (const char *bencoded_value);
char *encode_dict_bencode (bencode *bencoded_dict);
char *encode_list_bencode (bencode *lst);

#include <stdarg.h>
#include <stddef.h>

bencode_dictionary *get_dictionary_from_key (bencode *b, const char *key);
bencode *decode_bencode (const char *bencoded_value);
void bencode_free (bencode *b);

size_t bencode_to_string (bencode *b, char *buffer, size_t size);
void bencode_json (bencode *b);
/**
 *  key_sum counter of all keys
 *  Example:
 *          You want to print "length" key which set in dictionary whether
 *          value of main dictionary with key "info" You must use that
 *          (bencode* b,2,"info","length") if it more nested
 * dictionary key "length" have another dictionary also how "info" use it
 * likewise docs above just add n keys and make key_sum n
 */
void bencode_print_dict_from_key (bencode *b, int key_sum, const char *key,
                                  ...);

#endif
