// io.h
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef IO_H
#define IO_H

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

void print_strings(int fd, size_t str_count, ...);
void print_error(size_t str_count, ...);

#endif	// IO_H