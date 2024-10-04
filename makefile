# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g -Werror -std=gnu99

EXEC = myshell

SRC = shell.c

OBJ = $(SRC:.c=.o)

$(EXEC):	$(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJ) $(EXEC)
