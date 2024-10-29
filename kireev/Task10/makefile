all:
	gcc -Wall task10.c -o task
	touch datafile
	echo "Test text:\nHello World!\n" > datafile
	-./task cat datafile
	rm task && rm datafile