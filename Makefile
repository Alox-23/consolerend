SRC = main.c consoleRenderer.c input.c
FLAGS = -Wall -Wextra -O2 
CC = gcc
TARGET = main

all:
	$(CC) $(FLAGS) -o $(TARGET) $(SRC)
	./$(TARGET)

run:
	./$(TARGET)

comp:
	$(CC) $(FLAGS) -o $(TARGET) $(SRC)
