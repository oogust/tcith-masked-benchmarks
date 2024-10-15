#XCHAIN?=
CC?=gcc
XCHAIN?=riscv64-unknown-elf-
#CC=riscv64-unknown-elf-gcc
WARNING_FLAGS?=-Wall -Wextra -Wpedantic -Wshadow
#CFLAGS=-O3
CFLAGS = -O3 -Wall -g -mabi=ilp32 -march=rv32imc -I../
#CFLAGS?= -flto -fPIC -std=c11 -I../
#CFLAGS= -O3 -Wall -g -mabi=ilp32 -march=rv32imc

OBJ = util.o rng.o
PRIM = aes_coron keccak_plain keccak_bs mult

all : $(OBJ) $(PRIM) libprim

%.o : %.c
	$(CC) -c $(WARNING_FLAGS) $(CFLAGS) -I. $< -o $@

aes_coron : 
	cd aes-coron && CC=$(XCHAIN)gcc CFLAGS="$(CFLAGS)" make obj

keccak_plain : 
	cd masked-keccak-saber-plain && CC=$(XCHAIN)gcc CFLAGS="$(CFLAGS)" make obj

keccak_bs : 
	cd masked-keccak-saber-bitslice && CC=$(XCHAIN)gcc CFLAGS="$(CFLAGS)" make obj

mult : 
	cd field-multiplication && CC=$(XCHAIN)gcc CFLAGS="$(CFLAGS)" make obj

libprim :
	ar rcs libprim.a $(wildcard *.o) $(wildcard ./*/*.o)

clean :
	cd aes-coron && CC=$(XCHAIN)gcc make clean
	cd field-multiplication && CC=$(XCHAIN)gcc make clean
	cd masked-keccak-saber-bitslice && CC=$(XCHAIN)gcc make clean
	cd masked-keccak-saber-plain && CC=$(XCHAIN)gcc make clean
	rm -f *.o $(EXE) libprim.a