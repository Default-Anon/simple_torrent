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

char *
write_file_content_to_buffer (const char *file_name)
{
#define DEFAULT_BUFFER_READ_VALUE 1024
  int buffer_size = DEFAULT_BUFFER_READ_VALUE, symbol = 0, counter = 0;
  int file_fd = open (file_name, O_RDONLY);
  char *buf;
  if (file_fd == -1)
    {
      perror ("fopen error:");
      exit (0);
    }

  buf = (char *)malloc (sizeof (char) * buffer_size);
  symbol = read (file_fd, buf, DEFAULT_BUFFER_READ_VALUE);
  while (symbol)
    {
      if (symbol == -1)
        {
          perror ("read error:");
          exit (0);
        }
      buffer_size += buffer_size;
      buf = realloc (buf, buffer_size);
      symbol = read (file_fd, buf + buffer_size - DEFAULT_BUFFER_READ_VALUE,
                     DEFAULT_BUFFER_READ_VALUE);
    }
  close (file_fd);
  return buf;
}
