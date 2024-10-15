CC?=gcc
CFLAGS?= -O3

AESOBJ = aes.o aes_rp.o share.o aes_share.o runaes.o common.o #aes_rp_prg.o prg.o prgmat.o

all : runaes.out 

obj : $(AESOBJ)

runaes.out : $(AESOBJ)
	$(CC) $(CFLAGS) -o $@ $(AESOBJ)

%.o: %.c 
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o *.out
