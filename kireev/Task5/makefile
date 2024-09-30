all:
	gcc -Wall task5.c -o task
	touch datafile
	echo "Test text:\nHello World!\n" > datafile
	-./task datafile
	rm task && rm datafile