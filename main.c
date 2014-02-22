// main.c
// Author: Dmitry Kukovinets (d1021976@gmail.com)

#include <unistd.h>

#include "io.h"
#include "copy.h"

const char *pname;

void print_usage(void)
{
	print_strings(2, 5, "Invalid argument! Usage:\n\t", pname, " [-R] SRC_FILE DEST_FILE\n\t",
														pname, " [-R] SRC_FILE1|SRC_DIR1 [SRC_FILE2|SRC_DIR2] DEST_DIR");
}

int main(int argc, char **argv)
{
	pname = argv[0];
	char recursive = 0;
	int c;
	while ((c = getopt(argc, argv, "R")) != -1) {
		switch (c) {
		case 'R':
			recursive = 1;
			break;
		case '?': default:
			print_usage();
			return 10;
		}
	}
	argc -= optind;
	argv += optind;
	
	if (argc < 2) {
		print_usage();
		return 11;
	}
	
	const char *dest_path = argv[argc - 1];
	const char **p;
	for (p = (const char **)argv; *p != dest_path; ++p)
		copy(*p, dest_path, recursive);
	
	return 0;
}