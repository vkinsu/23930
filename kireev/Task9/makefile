all:
	gcc -Wall task9.c -o task
	touch datafile
	echo "Test text:\nHello World!\n" > datafile
	-./task datafile
	rm task && rm datafile