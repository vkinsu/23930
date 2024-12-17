all:
	gcc -Wall task31_s.c -o task_s
	gcc -Wall task31_c.c -o task_c
	-./task_s
solaris:
	gcc -Wall task31_s.c -o task_s -lsocket
	gcc -Wall task31_c.c -o task_c -lsocket
	-./task_s
connect:
	-./task_c
clear:
	rm task_s
	rm task_c
