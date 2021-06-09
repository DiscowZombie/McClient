CFLAGS = -g -Wall -Wextra -Werror -Wvla -lm -Iincludes
LDFLAGS = -lm
CC = gcc

SRC = $(wildcard src/*.c)
OBJ	= $(SRC:.c=.o)
BINPATH = bin
NAME = mc-client

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)
	rm -rf $(OBJ)
	mkdir -p $(BINPATH)/
	mv $@ $(BINPATH)/

clean:
	rm -rf $(OBJ) $(BINPATH)

re: clean all
