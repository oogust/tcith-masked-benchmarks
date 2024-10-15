CC?=gcc
#CC=riscv64-unknown-elf-gcc
#WARNING_FLAGS?=-Wall -Wextra -Wpedantic -Wshadow
#CFLAGS=-O3
CFLAGS?= -flto -fPIC -std=c11 -I../
#CFLAGS= -O3 -Wall -g -mabi=ilp32 -march=rv32imc

OBJ = masked_fips202.o #masked_representations.o 
UTIL = ../rng.o ../util.o

all : bench

obj : $(OBJ)

bench : bench.c $(OBJ)
	$(CC) $(WARNING_FLAGS) $(CFLAGS) -o bench bench.c $(OBJ) $(UTIL)

%.o : %.c
	$(CC) -c $(WARNING_FLAGS) $(CFLAGS) -I. $< -o $@

clean :
	rm -f *.o $(EXE)