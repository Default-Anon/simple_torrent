#ifndef __UTILS_H
#define __UTILS_H

#include <stdbool.h>
#include <stddef.h>
bool is_str_digit_compatible (const char *str);
bool is_str_number_overflow (const char *str);
bool is_digit (char c);

char *write_file_content_to_buffer (const char *file_name);

#endif
