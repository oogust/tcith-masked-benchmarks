CC=gcc-10
WARNING_FLAGS=-Wall -Wextra -Wpedantic -Wshadow
CFLAGS= -O3 -march=native $(WARNING_FLAGS) -std=gnu99

INCLUDE_UNITS=../../../units/

all: units

units: main.c ../gf251.c test-data.h
	$(CC) $(CFLAGS) main.c ../gf251.c -I.. -I$(INCLUDE_UNITS) -DNO_RND -o $@

pow251: pow251.c ../gf251.c
	$(CC) $(CFLAGS) pow251.c ../gf251.c -I.. -I$(INCLUDE_UNITS) -DNO_RND -o $@

## To clean
clean:
	rm -f units pow251
