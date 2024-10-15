CC=gcc
WARNING_FLAGS=-Wall -Wextra -Wpedantic -Wshadow
CFLAGS= -O3 -march=native $(WARNING_FLAGS) -std=gnu99

INCLUDE_UNITS=../../../units/

all: units

units: main.c ../gf256.c
	$(CC) $(CFLAGS) main.c ../gf256.c -I.. -I$(INCLUDE_UNITS) -DNO_RND -o $@

pow256: pow256.c ../gf256.c
	$(CC) $(CFLAGS) pow256.c ../gf256.c -I.. -I$(INCLUDE_UNITS) -DNO_RND -o $@

test: test.c
	$(CC) $(CFLAGS) test.c ../gf256.c -I.. -I$(INCLUDE_UNITS) -DNO_RND -o $@

## To clean
clean:
	rm -f units pow256 test
