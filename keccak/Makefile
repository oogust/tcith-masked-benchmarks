CC?=gcc
WARNING_FLAGS?=-Wall -Wextra -Wpedantic -Wshadow
CFLAGS?= -O3 -flto -fPIC -std=c11

LIB_OBJ = fips202-masked.o masksONOFF.o

all : $(LIB_OBJ)

fips202-masked.o : fips202-masked.c
	$(CC) $(WARNING_FLAGS) $(CFLAGS) -c fips202-masked.c

masksONOFF.o : masksONOFF.c
	$(CC) $(WARNING_FLAGS) $(CFLAGS) -c masksONOFF.c

clean :
	rm -f *.o