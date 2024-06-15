#ifndef __UTILS_H
#define __UTILS_H

#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

bool is_str_digit_compatible (const char *str);
bool is_str_number_overflow (const char *str);
bool is_digit (char c);

unsigned char *write_file_content_to_buffer (const unsigned char *file_name);

#endif
