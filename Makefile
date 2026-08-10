NAME	= codexion

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread
INCLUDES = -I.

SRCS	= srcs/cleaning.c srcs/coder.c srcs/codexion.c srcs/dongle.c \
		  srcs/init.c srcs/monitor.c srcs/params.c srcs/scheduler.c \
		  srcs/threads.c srcs/time.c srcs/utils.c
OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c codexion.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re