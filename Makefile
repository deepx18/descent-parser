CC=gcc
FLAGS= -Wall -Wextra -ggdb

main: main.c
	$(CC) $(FLAGS) -o main main.c
