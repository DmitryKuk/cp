// simple-main.c
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include "io.h"
#include "copy.h"

int main(int argc, char **argv)
{
	if (argc != 3) {
		print_strings(STDERR_FILENO, 3, "Usage: ", argv[0], " SRC_FILE DEST_FILE");
		return 6;
	}
	return copy_file_file(argv[1], argv[2], S_IRUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
}