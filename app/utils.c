#include "utils.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool
is_digit (char c)
{
  return c >= '0' && c <= '9';
}
bool
is_str_digit_compatible (const char *str)
{
  while (*str)
    {
      if (!is_digit (*str))
        {
          return false;
        }
      ++str;
    }
  return true;
}
bool
is_str_number_overflow (const char *str)
{
  long long int str_int = strtoll (str, NULL, 10);
  char checker[512];
  sprintf (checker, "%lld", str_int);
  if (strcmp (str, checker))
    return true;
  return false;
}

unsigned char *
write_file_content_to_buffer (const unsigned char *file_name)
{
  FILE *file_fd = fopen (file_name, "r");
  fseek (file_fd, 0, SEEK_END);
  long file_sz = ftell (file_fd);
  fseek (file_fd, 0, SEEK_SET);
  unsigned char *buf;
  if (file_fd == NULL)
    {
      perror ("fopen error:");
      exit (0);
    }

  buf = (unsigned char *)malloc (sizeof (unsigned char) * file_sz);
  memset (buf, 0, file_sz);
  size_t bytes_read = fread (buf, sizeof (unsigned char), file_sz, file_fd);
  if (bytes_read != file_sz)
    {
      fprintf (stderr, "Failed to read file.\n");
      fclose (file_fd);
      return NULL;
    }

  fclose (file_fd);
  return buf;
}
