tty-switcherd: switcher.c switcher.h
	$(CC) -std=c2x -Wall -Wextra -Werror -lhidapi-hidraw -o $@ $<
