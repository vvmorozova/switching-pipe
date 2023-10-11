FILES	= main1.c
# -Werror
FLAGS	= -Wall -Wextra `pkg-config --cflags --libs gstreamer-1.0 ` 

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
graph:
	dot -Tpng debug-pics/pipeline.dot > debug-pics/out.png
run:
	rm debug-pics/pipeline.dot; export GST_DEBUG_DUMP_DOT_DIR=/home/morozovavv/Documents/switching-pipe/debug-pics; GST_DEBUG=3 ./switching_pipe rtsp://192.169.0.200/stream0 kitten.mp4 50
.PHONY : all, clean, fclean, re