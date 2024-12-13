all:
	gcc -Wall task30_s.c -o task_s
	gcc -Wall task30_c.c -o task_c
	-./task_s &
	-./task_c
	rm task_s
	rm task_c
solaris:
	gcc -Wall task30_s.c -o task_s -lsocket
	gcc -Wall task30_c.c -o task_c -lsocket
	-./task_s &
	-./task_c
	rm task_s
	rm task_c
