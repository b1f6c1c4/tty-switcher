CFLAGS+=-std=c2x -Wall -Wextra -Werror

tty-switcherd: switcher.o
	$(CC) $(LDFLAGS) -o $@ $^ -lhidapi-hidraw

switcher.o: switcher.c switcher.h
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f tty-switcherd switcher.o

.PHONY: clean
