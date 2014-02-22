// other.h
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef OTHER_H
#define OTHER_H

#include <string.h>
#include <limits.h>

size_t filename(const char *path);
	// Returns position of filename in path (or ULLONG_MAX if path == NULL)
	// Example: filename("/hello/world.txt") == 7
	// So (path + filename(path)) is pointer to filename string begin.

size_t dirname(const char *path);
	// Returns position of filename in path (or ULLONG_MAX if path == NULL)
	// Example: filename("/hello/world.txt") == 7
	// So (path + filename(path)) is pointer to filename string begin.

#endif	// OTHER_H