This program is unix cp analog. It supports '-R' option to copy directories.

1. Full version
To build it run:
	cd cp/ && make

To use run:
	./cp [-R] SRC1 [SRC2 ...] DEST

Options:
	-R  Recursively process directories

To clear run:
	make clear

2. Simple version
There is also a simple variant without directories support.
To build it run:
	cd cp/ && make simple

To use run:
	./simple-cp SRC DEST

To clear run:
	rm simple-cp && make clear