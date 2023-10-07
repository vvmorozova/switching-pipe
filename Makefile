FILES	= main.c
# -Werror
FLAGS	= -Wall -Wextra `pkg-config --cflags --libs gstreamer-1.0`

NAME	= switching_pipe

OBJ		= $(FILES:.c=.o)

HEADER	= switching_pipe.h

CC		= gcc

all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	libtool --mode=link $(CC) $(FLAGS) $(OBJ) -o $(NAME)
	@echo "binary is created"
	

%.o : %.c $(HEADER)
	${CC} $(FLAGS) -c $< -o $@ -I.$(HEADER)
	echo "Compilation $<"

clean:
	@rm -f $(OBJ)
	@echo "Obj deleted"

fclean: clean
	@rm -f $(NAME)
	@echo "All deleted"

re: fclean all

.PHONY : all, clean, fclean, re