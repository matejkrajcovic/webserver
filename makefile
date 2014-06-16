all:
	gcc -Wextra -Wall -Wfloat-equal main.c parser.c -o webserver

run:
	./webserver 80 rootDir
