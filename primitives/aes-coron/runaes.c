// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 2 as published
// by the Free Software Foundation.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "aes.h"
#include "aes_rp.h"
#include "share.h"
#include "aes_share.h"
#include "common.h"


void printMes(char *s,byte *m)
{
  printf("%s=",s);
  int i;
  for(i=0;i<16;i++) printf("%02x",m[i]);
  printf("\n");
}

int main()
{
  int n;
  int nt=1;
  int i;
  byte keyex[16]={0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};

  byte inex[16]={0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34};

  byte outex[16]={0x39,0x25,0x84,0x1d,0x02,0xdc,0x09,0xfb,0xdc,0x11,0x85,0x97,0x19,0x6a,0x0b,0x32};

  byte in[16],out[16];
  byte key[16];

  //printMes("in",inex);
  //printMes("key",keyex);

  for(i=0;i<16;i++) key[i]=keyex[i];
  for(i=0;i<16;i++) in[i]=inex[i];

  int dt,base;

  printf("Without countermeasure, plain: ");
  base=run_aes(&aes,in,out,key,outex,nt,0);

  printf("Without countermeasure, RP: ");
  run_aes(&aes_rp,in,out,key,outex,nt,base);

  for(n=1;n<=32;n+=1)
  {
    
    printf("n=%d\n",n);
    printf("  With RP countermeasure: ");
    run_aes_share(in,out,key,outex,n,&subbyte_rp_share,nt,base);
    
  }
}


