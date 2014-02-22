// copy.h
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#ifndef COPY_H
#define COPY_H

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>

#include "io.h"
#include "other.h"

#define BUFFER_SIZE 2048


// Output strings
#define ALLOCATION_ERROR		"Allocation error"

#define CANT_OPEN_FILE_			"Can't open file: "
#define CANT_OPEN_DIR_			"Can't open directory: "

#define CANT_READ_FROM_FILE_	"Can't read from file: "
#define CANT_WRITE_TO_FILE_		"Can't write to file: "

#define CANT_GET_STATUS_		"Can't get status: "
#define CANT_SET_PERMISSIONS_	"Can't set permissions: "

#define CANT_COPY_DIR_			"Can't copy directory (without -R): "

// Return codes
#define OK					0

#define ERROR_OPEN_READ		1
#define ERROR_OPEN_WRITE	2

#define ERROR_READ			3
#define ERROR_WRITE			4

#define ERROR_ALLOCATION	5
#define ERROR_GET_STATUS	6

#define ERROR_NEED_RECURS	7

int copy(const char *src_path, const char *dest_path, char recursive);
int copy_file_file(const char *src_filename, const char *dest_filename, mode_t dest_file_mode);

#endif	// COPY_H