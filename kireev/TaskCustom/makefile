all:
	gcc -Wall custom_s.c -o custom_s
	gcc -Wall custom_r.c -o custom_r
	gcc -Wall custom_t.c -o custom_t
	-./custom_s &
	-./custom_r
solaris:
	gcc -Wall custom_s.c -o custom_s -lsocket
	gcc -Wall custom_r.c -o custom_r -lsocket
	gcc -Wall custom_t.c -o custom_t -lsocket
	-./custom_s &
	-./custom_r
connect:
	-./custom_t
clear:
	rm custom_s
	rm custom_r
	rm custom_t
