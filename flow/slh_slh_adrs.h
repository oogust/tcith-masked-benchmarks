		  : //  slh_adrs.h
		  : //  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.
		  : 
		  : //  === Internal ADRS manipulation functions (Section 4.2)
		  : 
		  : #ifndef _SLH_ADRS_H_
		  : #define _SLH_ADRS_H_
		  : 
		  : #include "slh_param.h"
		  : #include <string.h>
		  : 
		  : //  ADRS type constants
		  : 
		  : #define ADRS_WOTS_HASH  0
		  : #define ADRS_WOTS_PK    1
		  : #define ADRS_TREE       2
		  : #define ADRS_FORS_TREE  3
		  : #define ADRS_FORS_ROOTS 4
		  : #define ADRS_WOTS_PRF   5
		  : #define ADRS_FORS_PRF   6
		  : 
		  : //  Algorithm 1: toInt(x, n)
		  : 
      264 : static inline uint64_t slh_toint(const uint8_t *x, unsigned n)
		  : {
		  :     unsigned i;
		  :     uint64_t t;
		  : 
       15 :     if (n == 0)
		  :         return 0;
        9 :     t = (uint64_t) x[0];
       66 :     for (i = 1; i < n; i++) {
       84 :         t <<= 8;
      105 :         t += (uint64_t) x[i];
		  :     }
		  :     return t;
		  : }
		  : 
		  : //  Algorithm 2: toByte(x, n)
		  : 
		  : static inline void slh_tobyte(uint8_t *x, uint64_t t, unsigned n)
		  : {
		  :     unsigned i;
		  : 
      174 :     if (n == 0)
		  :         return;
      609 :     for (i = n - 1; i > 0; i--) {
       87 :         x[i] = (uint8_t) (t & 0xFF);
      348 :         t >>= 8;
		  :     }
       87 :     x[0] = (uint8_t) t;
		  : }
		  : 
		  : //  === Clear / initialize
		  : static inline void adrs_zero(slh_ctx_t *ctx)
		  : {
        7 :     ctx->adrs->u32[0] = 0;
        7 :     ctx->adrs->u32[1] = 0;
        7 :     ctx->adrs->u32[2] = 0;
        7 :     ctx->adrs->u32[3] = 0;
        7 :     ctx->adrs->u32[4] = 0;
        7 :     ctx->adrs->u32[5] = 0;
        7 :     ctx->adrs->u32[6] = 0;
        7 :     ctx->adrs->u32[7] = 0;
		  : }
		  : 
		  : //  === Set layer address.
		  : static inline void adrs_set_layer_address(slh_ctx_t *ctx, uint32_t x)
		  : {
      712 :     ctx->adrs->u32[0] = rev8_be32(x);
		  : }
		  : 
		  : //  === Set tree addresss.
		  : static inline void adrs_set_tree_address(slh_ctx_t *ctx, uint64_t x)
		  : {
		  :     //  bytes a[4:8] of tree address are always zero
      672 :     ctx->adrs->u32[2] = rev8_be32(x >> 32);
      663 :     ctx->adrs->u32[3] = rev8_be32(x & 0xFFFFFFFF);
		  : }
		  : 
		  : //  === Set key pair Address.
		  : static inline void adrs_set_key_pair_address(slh_ctx_t *ctx, uint32_t x)
		  : {
     3144 :     ctx->adrs->u32[5] = rev8_be32(x);
		  : }
		  : 
		  : //  === Get key pair Address.
		  : static inline uint32_t adrs_get_key_pair_address(const slh_ctx_t *ctx)
		  : {
		  :     return rev8_be32(ctx->adrs->u32[5]);
		  : }
		  : 
		  : //  === Set FORS tree height.
		  : static inline void adrs_set_tree_height(slh_ctx_t *ctx, uint32_t x)
		  : {
    90559 :     ctx->adrs->u32[6] = rev8_be32(x);
		  : }
		  : 
		  : //  === Set WOTS+ chain address.
		  : static inline void adrs_set_chain_address(slh_ctx_t *ctx, uint32_t x)
		  : {
		  :     ctx->adrs->u32[6] = rev8_be32(x);
		  : }
		  : 
		  : //  === Set FORS tree index.
		  : static inline void adrs_set_tree_index(slh_ctx_t *ctx, uint32_t x)
		  : {
    60172 :     ctx->adrs->u32[7] = rev8_be32(x);
		  : }
		  : 
		  : //  === Get FORS tree index.
		  : static inline uint32_t adrs_get_tree_index(const slh_ctx_t *ctx)
		  : {
		  :     return rev8_be32(ctx->adrs->u32[7]);
		  : }
		  : 
		  : //  === Set WOTS+ hash address.
		  : static inline void adrs_set_hash_address(slh_ctx_t *ctx, uint32_t x)
		  : {
		  :     ctx->adrs->u32[7] = rev8_be32(x);
		  : }
		  : 
		  : static inline void adrs_set_type(slh_ctx_t *ctx, uint32_t y)
		  : {
    29880 :     ctx->adrs->u32[4] = rev8_be32(y);
		  : }
		  : 
		  : static inline uint32_t adrs_get_type(slh_ctx_t *ctx)
		  : {
		  :     return rev8_be32(ctx->adrs->u32[4]);
		  : }
		  : 
		  : //  === "Function ADRS.setTypeAndClear(Y) for addresses sets the type
		  : //  of the ADRS to Y and sets the final 12 bytes of the ADRS to zero."
		  : static inline void adrs_set_type_and_clear(slh_ctx_t *ctx, uint32_t y)
		  : {
      455 :     ctx->adrs->u32[4] = rev8_be32(y);
      160 :     ctx->adrs->u32[5] = 0;
      160 :     ctx->adrs->u32[6] = 0;
      160 :     ctx->adrs->u32[7] = 0;
		  : }
		  : 
		  : static inline void adrs_set_type_and_clear_not_kp(slh_ctx_t *ctx, uint32_t y)
		  : {
     6378 :     ctx->adrs->u32[4] = rev8_be32(y);
      320 :     ctx->adrs->u32[6] = 0;
      320 :     ctx->adrs->u32[7] = 0;
		  : }
		  : 
		  : 
		  : //  === Compressed 22-byte address ADRSc used with SHA-2.
		  : static inline void adrsc_22(const slh_ctx_t *ctx, uint8_t *ac)
		  : {
		  :     int i;
		  :     ac[0] = ctx->adrs->u8[3];
		  :     for (i = 0; i < 8; i++) {
		  :         ac[i + 1] = ctx->adrs->u8[i + 8];
		  :     }
		  :     for (i = 0; i < 13; i++) {
		  :         ac[i + 9] = ctx->adrs->u8[i + 19];
		  :     }
		  : }
		  : 
		  : //  _SLH_ADRS_H_
		  : #endif
		  : 
