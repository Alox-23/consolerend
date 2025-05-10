all:
	gcc -Wall -Wextra -O2 -o main main.c consoleRenderer.c
	./main
