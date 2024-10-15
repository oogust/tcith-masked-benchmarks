CC=gcc-10
WARNING_FLAGS=-Wall -Wextra -Wpedantic -Wshadow
CFLAGS= -O3 -march=native $(WARNING_FLAGS) -std=gnu99

barrett: barrett.c
	$(CC) $(CFLAGS) barrett.c -o $@

clean:
	rm -f barrett
