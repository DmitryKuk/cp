// other.c
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "other.h"

size_t filename(const char *path)
{
	if (path == NULL) return ULLONG_MAX;
	
	size_t pos = 0, i = 0;
	while (path[i])
		if (path[i++] == '/') pos = i;
	return pos;
}

size_t dirname(const char *path)
{
	if (path == NULL) return ULLONG_MAX;
	
	size_t pos = 0, i = 0;
	while (path[i])
		if (path[i++] == '/' && path[i]) pos = i;
	return pos;
}