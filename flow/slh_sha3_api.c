		  : //  sha3_api.c
		  : //  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.
		  : 
		  : //  === FIPS 202: SHA-3 hash and SHAKE eXtensible Output Functions (XOF)
		  : //      Hash padding mode code for testing permutation implementations.
		  : 
		  : #ifndef SLOTH_NO_SHA3
		  : #include "sha3_api.h"
		  : 
		  : //  These functions have not been optimized for performance -- they are
		  : //  here just to facilitate testing of the permutation code implementations.
		  : 
		  : //  initialize the context for SHA3
		  : 
		  : void sha3_init(sha3_ctx_t *c, int mdlen)
       24 : {
		  :     int i;
		  : 
		  :     for (i = 0; i < 25; i++)
       16 :         c->st.d[i] = 0;
        4 :     c->mdlen = mdlen;           //  in SHAKE; if 0, padding done
       16 :     c->rsiz = 200 - 2 * mdlen;
        4 :     c->pt = 0;
       20 : }
		  : 
		  : //  update state with more data
		  : 
		  : void sha3_update(sha3_ctx_t *c, const void *data, size_t len)
       60 : {
		  :     size_t i;
		  :     int j;
		  : 
       15 :     j = c->pt;
      736 :     for (i = 0; i < len; i++) {
     1848 :         c->st.b[j++] ^= ((const uint8_t *) data)[i];
      616 :         if (j >= c->rsiz) {
		  :             keccak_f1600(c->st.d);
		  :             j = 0;
		  :         }
		  :     }
       15 :     c->pt = j;
       60 : }
		  : 
		  : //  finalize and output a hash
		  : 
		  : void sha3_final(sha3_ctx_t *c, uint8_t *md)
		  : {
		  :     int i;
		  : 
		  :     c->st.b[c->pt] ^= 0x06;
		  :     c->st.b[c->rsiz - 1] ^= 0x80;
		  :     keccak_f1600(c->st.d);
		  : 
		  :     for (i = 0; i < c->mdlen; i++) {
		  :         md[i] = c->st.b[i];
		  :     }
		  : }
		  : 
		  : //  compute a SHA-3 hash "md" of "mdlen" bytes from data in "in"
		  : 
		  : void *sha3(uint8_t *md, int mdlen, const void *in, size_t inlen)
		  : {
		  :     sha3_ctx_t sha3;
		  : 
		  :     sha3_init(&sha3, mdlen);
		  :     sha3_update(&sha3, in, inlen);
		  :     sha3_final(&sha3, md);
		  : 
		  :     return md;
		  : }
		  : 
		  : //  SHAKE128 and SHAKE256 extensible-output functionality
		  : //  squeeze output
		  : 
		  : void shake_out(sha3_ctx_t *c, uint8_t *out, size_t len)
       32 : {
		  :     size_t  i;
		  :     int j;
		  : 
		  :     //  add padding on the first call
        8 :     if (c->mdlen != 0) {
       20 :         c->st.b[c->pt] ^= 0x1F;
       24 :         c->st.b[c->rsiz - 1] ^= 0x80;
        8 :         keccak_f1600(c->st.d);
        4 :         c->pt = 0;
       12 :         c->mdlen = 0;
		  :     }
		  : 
		  :     j = c->pt;
      248 :     for (i = 0; i < len; i++) {
      236 :         if (j >= c->rsiz) {
		  :             keccak_f1600(c->st.d);
		  :             j = 0;
		  :         }
      590 :         out[i] = c->st.b[j++];
		  :     }
        4 :     c->pt = j;
       24 : }
		  : 
		  : //  SLOTH_NO_SHA3
		  : #endif
