NAME	= codexion

CC		= cc
CFLAGS	= -g3 -Wall -Wextra -Werror -pthread
INCLUDES = -I.

SRCS	= srcs/cleaning.c srcs/coder.c srcs/codexion.c srcs/dongle.c \
		  srcs/init.c srcs/monitor.c srcs/params.c srcs/scheduler.c \
		  srcs/threads.c srcs/time.c srcs/utils.c srcs/error.c \
		  srcs/init2.c srcs/heap.c srcs/heap_utils.c
OBJS	= $(SRCS:.c=.o)

all: $(NAME) 

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c codexion.h Makefile
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re