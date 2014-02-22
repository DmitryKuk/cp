// io.c
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "io.h"

void print_strings(int fd, size_t str_count, ...)
{
	va_list vl;
	va_start(vl, str_count);
	size_t i;
	for (i = 0; i < str_count; ++i) {
		const char *str = va_arg(vl, const char *);
		write(fd, str, strlen(str));
	}
	va_end(vl);
	write(fd, "\n", 1);
}

void print_error(size_t str_count, ...)
{
	va_list vl;
	va_start(vl, str_count);
	size_t i;
	for (i = 1; i < str_count; ++i) {
		const char *str = va_arg(vl, const char *);
		write(STDERR_FILENO, str, strlen(str));
	}
	if (i == str_count) perror(va_arg(vl, const char *));
	else perror(NULL);
	va_end(vl);
}