all:
	mkdir -p bin
	gcc -Wall -Wfloat-equal main.c parser.c server.c -o ./bin/webserver

run:
	./bin/webserver 2014 root
