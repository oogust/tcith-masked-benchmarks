		  : //  slh_dsa.c
		  : //  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.
		  : 
		  : //  === FIPS 205 (ipd) Stateless Hash-Based Digital Signature Standard
		  : 
		  : #include "slh_dsa.h"
		  : #include "slh_ctx.h"
		  : #include "slh_adrs.h"
		  : #include <assert.h>
		  : 
		  : //  === Internal
		  : 
		  : //  helper functions to compute "len = len1 + len2"
		  : 
		  : static inline uint32_t get_len1(const slh_param_t *prm)
		  : {
     4919 :     return ((8 * prm->n + prm->lg_w - 1) / prm->lg_w);
		  : }
		  : 
		  : static inline uint32_t get_len2(const slh_param_t *prm)
		  : {
		  : #ifdef NDEBUG
		  :     (void) prm;
		  : #endif
		  :     //  Appedix B:
		  :     //  "When lg_w = 4 and 9 <= n <= 136, the value of len2 will be 3."
		  :     assert(prm->lg_w == 4 && prm->n >= 9 && prm->n <= 136);
		  :     return 3;
		  : }
		  : static inline uint32_t get_len(const slh_param_t *prm)
		  : {
     3856 :     return  get_len1(prm) + get_len2(prm);
		  : }
		  : 
		  : //  Return signature size in bytes for parameter set *prm.
		  : size_t slh_sig_sz(const slh_param_t *prm)
		  : {
       44 :     return  (1 + prm->k*(1 + prm->a) + prm->h + prm->d * get_len(prm)) * prm->n;
       11 : }
		  : 
		  : //  === Compute the base 2**b representation of X.
		  : //  Algorithm 3: base_2b(X, b, out_len)
		  : 
		  : static inline size_t base_2b(   uint32_t *v, const uint8_t *x,
		  :                                 uint32_t b, size_t v_len)
		  : {
		  :     size_t i, j;
		  :     uint32_t l, t, m;
		  : 
        4 :     j = 0;
        4 :     l = 0;
        4 :     t = 0;
        8 :     m = (1 << b) - 1;
      328 :     for (i = 0; i < v_len; i++) {
      232 :         while (l < b) {
      500 :             t = (t << 8) + x[j++];
      100 :             l += 8;
		  :         }
      132 :         l -= b;
      396 :         v[i] = (t >> l) & m;
		  :     }
		  :     return j;
		  : }
		  : 
		  : //  little bit faster version for b = 4
		  : 
		  : static inline size_t base_16(   uint32_t *v, const uint8_t *x, int v_len)
		  : {
		  :     int i, j, l, t;
		  : 
		  :     j = 0;
     4785 :     for (i = 0; i < v_len - 2; i += 2) {
     1305 :         t = x[j++];
     2784 :         v[i]     = t >> 4;
     2784 :         v[i + 1] = t & 0xF;
		  :     }
		  : 
       87 :     l = 0;
       87 :     t = 0;
      783 :     for (; i < v_len; i++) {
      261 :         while (l < 4) {
      783 :             t = (t << 8) + x[j++];
		  :             l += 8;
		  :         }
      174 :         l -= 4;
      696 :         v[i] = (t >> l) & 0xF;
		  :     }
		  :     return j;
		  : }
		  : 
		  : //  === Chaining function used in WOTS+
		  : //  Algorithm 4: chain(X, i, s, PK.seed, ADRS)
		  : //  (see prm->chain)
		  : 
		  : //  === Generate a WOTS+ public key.
		  : //  Algorithm 5: wots_PKgen(SK.seed, PK.seed, ADRS)
		  : //  (see xmms_node)
		  : 
		  : //  === Generate a WOTS+ signature on an n-byte message.
		  : //  Algorithm 6: wots_sign(M, SK.seed, PK.seed, ADRS)
		  : 
		  : //  (wots_csum is a shared helper function for algorithms 6 and 7)
		  : static void wots_csum(uint32_t *vm, const uint8_t *m, const slh_param_t *prm)
       87 : {
		  :     uint32_t csum, i, t;
		  :     uint32_t len1, len2;
		  :     uint8_t buf[4];
		  : 
     1305 :     len1 = get_len1(prm);
		  :     len2 = get_len2(prm);
		  : 
		  :     //base_2b(vm, m, prm->lg_w, len1);
    13920 :     base_16(vm, m, len1);
		  : 
       87 :     csum = 0;
      435 :     t = (1 << prm->lg_w) - 1;
     8526 :     for (i = 0; i < len1; i++) {
     8352 :         csum += t - vm[i];
		  :     }
      435 :     csum <<= (8 - ((len2 * prm->lg_w) & 7)) & 7;
		  : 
      174 :     t = (len2 * prm->lg_w + 7) / 8;
       87 :     memset(buf, 0, sizeof(buf));
     1392 :     slh_tobyte(buf, csum, t);
		  : 
		  :     //base_2b(&vm[len1], buf, prm->lg_w, len2);
      696 :     base_16(&vm[len1], buf, len2);
      174 : }
		  : 
      220 : static size_t wots_sign(slh_ctx_t *ctx, uint8_t *sig, const uint8_t *m)
		  : {
       22 :     const slh_param_t *prm = ctx->prm;
		  :     uint32_t i, len;
		  :     uint32_t vm[SLH_MAX_LEN];
       22 :     size_t n = prm->n;
		  : 
      330 :     len = get_len(prm);
      110 :     wots_csum(vm, m, prm);
		  : 
     2464 :     for (i = 0; i < len; i++) {
     1540 :         adrs_set_chain_address(ctx, i);
     4620 :         prm->wots_chain(ctx, sig, vm[i]);
      770 :         sig += n;
		  :     }
		  :     return n * len;
      176 : }
		  : 
		  : //  === Compute a WOTS+ public key from a message and its signature.
		  : //  Algorithm 7: wots_PKFromSig(sig, M, PK.seed, ADRS)
		  : 
		  : static void wots_pk_from_sig(   slh_ctx_t *ctx, uint8_t *pk,
		  :                                 const uint8_t *sig,
		  :                                 const uint8_t *m)
     1040 : {
       65 :     const slh_param_t *prm = ctx->prm;
		  :     uint32_t i, t, len;
		  :     uint32_t vm[SLH_MAX_LEN];
		  :     uint8_t tmp[SLH_MAX_LEN * SLH_MAX_N];
       65 :     size_t n = prm->n;
		  :     size_t tmp_sz;
		  : 
      845 :     wots_csum(vm, m, prm);
		  : 
     1105 :     len = get_len(prm);
		  :     t = 15; // (1 << prm->lg_w) - 1;
		  :     tmp_sz = 0;
    12740 :     for (i = 0; i < len; i++) {
     2275 :         adrs_set_chain_address(ctx, i);
    20540 :         prm->chain( ctx, tmp + tmp_sz, sig + tmp_sz, vm[i], t - vm[i]);
		  :         tmp_sz += n;
		  :     }
		  : 
      325 :     adrs_set_type_and_clear_not_kp(ctx, ADRS_WOTS_PK);
      780 :     prm->h_t(ctx, pk, tmp, tmp_sz);
      910 : }
		  : 
		  : //  === Compute the root of a Merkle subtree of WOTS+ public keys.
		  : //  Algorithm 8: xmss_node(SK.seed, i, z, PK.seed, ADRS)
		  : 
		  : static void xmss_node(  slh_ctx_t *ctx, uint8_t *node,
		  :                         uint32_t i, uint32_t z)
     1206 : {
       67 :     const slh_param_t *prm = ctx->prm;
		  :     uint32_t j, k;
		  :     int p;
		  :     uint8_t *h0, h[SLH_MAX_HP][SLH_MAX_N];
		  :     uint8_t tmp[SLH_MAX_LEN * SLH_MAX_N];
		  :     uint8_t *sk;
      134 :     size_t n = prm->n;
     1005 :     size_t len = get_len(prm);
		  : 
       67 :     p = -1;
       67 :     i <<= z;
      709 :     for (j = 0; j < (1u << z); j++) {
		  : 
     1983 :         adrs_set_key_pair_address(ctx, i);
		  : 
		  :         //  === Generate a WOTS+ public key.
		  :         //  Algorithm 5: wots_PKgen(SK.seed, PK.seed, ADRS)
     1173 :         sk  = tmp;
    11664 :         for (k = 0; k < len; k++) {
    56700 :             adrs_set_chain_address(ctx, k);
    28350 :             prm->wots_chain(ctx, sk, 15);   //  w-1 =  (1 << prm->lg_w) - 1;
    11340 :             sk += n;
		  :         }
     6318 :         adrs_set_type_and_clear_not_kp(ctx, ADRS_WOTS_PK);
     1212 :         h0 = p >= 0 ? h[p] : node;
      394 :         p++;
     1547 :         prm->h_t(ctx, h0, tmp, len * n);
		  : 
		  :         //  this xmss_node() implementation is non-recursive
      984 :         for (k = 0; (j >> k) & 1; k++) {
      545 :             adrs_set_type_and_clear(ctx, ADRS_TREE);
     1045 :             adrs_set_tree_height(ctx, k + 1);
     1045 :             adrs_set_tree_index(ctx, i >> (k + 1));
       95 :             p--;
      216 :             h0 = p >= 1 ? h[p - 1] : node;
      475 :             prm->h_h(ctx, h0, h0, h[p]);
		  :         }
      162 :         i++;        //  advance index
		  :     }
     1072 : }
		  : 
		  : //  === Generate an XMSS signature.
		  : //  Algorithm 9: xmss_sign(M, SK.seed, idx, PK.seed, ADRS)
		  : 
		  : static size_t xmss_sign(slh_ctx_t *ctx, uint8_t *sx, const uint8_t *m,
		  :                         uint32_t idx)
      308 : {
		  : 
       22 :     const slh_param_t *prm = ctx->prm;
		  :     uint32_t j, k;
		  :     uint8_t *auth;
		  :     size_t sx_sz = 0;
       22 :     size_t n = prm->n;
		  : 
      550 :     sx_sz = get_len(prm) * n;
       22 :     auth = sx + sx_sz;
		  : 
      286 :     for (j = 0; j < prm->hp; j++) {
       66 :         k = (idx >> j) ^ 1;
      396 :         xmss_node(ctx, auth, k, j);
       66 :         auth += n;
		  :     }
      308 :     sx_sz += prm->hp * n;
		  : 
       88 :     adrs_set_type_and_clear_not_kp(ctx, ADRS_WOTS_HASH);
      286 :     adrs_set_key_pair_address(ctx, idx);
      110 :     wots_sign(ctx, sx, m);
		  : 
		  :     return sx_sz;
      264 : }
		  : 
		  : //  === Compute an XMSS public key from an XMSS signature.
		  : //  Algorithm 10: xmss_PKFromSig(idx, SIGXMSS, M, PK.seed, ADRS)
		  : 
		  : static void xmss_pk_from_sig(   slh_ctx_t *ctx, uint8_t *root, uint32_t idx,
		  :                                 const uint8_t *sig, const uint8_t *m)
      845 : {
		  : 
       65 :     const slh_param_t *prm = ctx->prm;
		  :     uint32_t k;
		  :     const uint8_t *auth;
       65 :     size_t n = prm->n;
		  : 
      260 :     adrs_set_type_and_clear_not_kp(ctx, ADRS_WOTS_HASH);
      845 :     adrs_set_key_pair_address(ctx, idx);
		  : 
      260 :     wots_pk_from_sig(ctx, root, sig, m);
      390 :     adrs_set_type_and_clear(ctx, ADRS_TREE);
		  : 
     1755 :     auth = sig + (get_len(prm) * n);
		  : 
     1040 :     for (k = 0; k < prm->hp; k++) {
		  : 
     2405 :         adrs_set_tree_height(ctx, k + 1);
     2145 :         adrs_set_tree_index(ctx, idx >> (k + 1));
		  : 
      585 :         if (((idx >> k) & 1) == 0) {
      975 :             prm->h_h(ctx, root, root, auth);
		  :         } else {
      379 :             prm->h_h(ctx, root, auth, root);
		  :         }
      195 :         auth += n;
		  :     }
      650 : }
		  : 
		  : 
		  : //  === Generate a hypertree signature.
		  : //  Algorithm 11: ht_sign(M, SK.seed, PK.seed, idx_tree, idx_leaf )
		  : 
		  : static size_t ht_sign(  slh_ctx_t *ctx, uint8_t *sh, uint8_t *m,
		  :                         uint64_t i_tree, uint32_t i_leaf)
		  : {
		  : 
        1 :     const slh_param_t *prm = ctx->prm;
		  :     uint32_t j;
		  :     size_t sx_sz;
		  : 
        8 :     adrs_zero(ctx);
        2 :     adrs_set_tree_address(ctx, i_tree);
        7 :     sx_sz = xmss_sign(ctx, sh, m, i_leaf);
		  : 
       69 :     for (j = 1; j < prm->d; j++) {
      147 :         xmss_pk_from_sig(ctx, m, i_leaf, sh, m);
       21 :         sh += sx_sz;
		  : 
       85 :         i_leaf = i_tree & ((1 << prm->hp) - 1);
      234 :         i_tree >>= prm->hp;
      233 :         adrs_set_layer_address(ctx, j);
      420 :         adrs_set_tree_address(ctx, i_tree);
      126 :         xmss_sign( ctx, sh, m, i_leaf);
		  :     }
		  : 
       10 :     return sx_sz * prm->d;
		  : }
		  : 
		  : 
		  : //  === Verify a hypertree signature.
		  : //  Algorithm 12: ht_verify(M, SIG_HT, PK.seed, idx_tree, idx_leaf, PK.root)
		  : 
		  : static bool ht_verify(  slh_ctx_t *ctx, const uint8_t *m,
		  :                         const uint8_t *sig_ht,
		  :                         uint64_t i_tree, uint32_t i_leaf)
		  : {
        4 :     const slh_param_t *prm = ctx->prm;
		  :     uint32_t i, j;
		  :     uint8_t node[SLH_MAX_N];
		  :     size_t st_sz;
		  : 
       16 :     adrs_zero(ctx);
        4 :     adrs_set_tree_address(ctx, i_tree);
		  : 
       14 :     xmss_pk_from_sig(ctx, node, i_leaf, sig_ht, m);
		  : 
       56 :     st_sz = (prm->hp + get_len(prm)) * prm->n;
      136 :     for (j = 1; j < prm->d; j++) {
      168 :         i_leaf = i_tree & ((1 << prm->hp) - 1);
      464 :         i_tree >>= prm->hp;
      466 :         adrs_set_layer_address(ctx, j);
      840 :         adrs_set_tree_address(ctx, i_tree);
       42 :         sig_ht += st_sz;
      252 :         xmss_pk_from_sig(ctx, node, i_leaf, sig_ht, node);
		  :     }
		  : 
		  :     uint8_t t;
        2 :     t = 0;
      108 :     for (i = 0; i < prm->n; i++) {
      128 :         t |= node[i] ^ ctx->pk_root[i];
		  :     }
        2 :     return t == 0;
		  : }
		  : 
		  : //  === Generate a FORS private-key value.
		  : //  Algorithm 13: fors_SKgen(SK.seed, PK.seed, ADRS, idx)
		  : 
		  : //  ( see prm->fors_hash() )
		  : 
		  : //  === Compute the root of a Merkle subtree of FORS public values.
		  : //  Algorithm 14: fors_node(SK.seed, i, z, PK.seed, ADRS)
		  : 
		  : static void fors_node(  slh_ctx_t *ctx, uint8_t *node,
		  :                         uint32_t i, uint32_t z)
		  : {
      198 :     const slh_param_t *prm = ctx->prm;
		  :     uint8_t h[SLH_MAX_A][SLH_MAX_N], *h0;
		  :     uint32_t j, k;
		  :     int p;
		  : 
      198 :     p = -1;
      198 :     i <<= z;
     4983 :     for (j = 0; j < (1u << z); j++) {
		  : 
		  : 
		  :         //  fors_SKgen() + hash
    22869 :         adrs_set_tree_index(ctx, i);
     9801 :         h0 = p >= 0 ? h[p] : node;
     5181 :         p++;
     8316 :         prm->fors_hash(ctx, h0, 1);
		  : 
		  :         //  this fors_node() implementation is non-recursive
    15774 :         for (k = 0; (j >> k) & 1; k++) {
    20691 :             adrs_set_tree_height(ctx, k + 1);
    22572 :             adrs_set_tree_index(ctx, i >> (k + 1));
     1881 :             p--;
     5148 :             h0 = p > 0 ? h[p - 1] : node;
     9405 :             prm->h_h(ctx, h0, h0, h[p]);
		  :         }
     2079 :         i++;        //  advance index
		  :     }
		  : }
		  : 
		  : 
		  : //  === Generate a FORS signature.
		  : //  Algorithm 15: fors_sign(md, SK.seed, PK.seed, ADRS)
		  : 
		  : static size_t fors_sign(slh_ctx_t *ctx, uint8_t *sf, const uint8_t *md)
        4 : {
        2 :     const slh_param_t *prm = ctx->prm;
		  :     uint32_t i, j, s;
		  :     uint32_t vi[SLH_MAX_K];
        2 :     size_t  n = prm->n;
		  : 
		  :     assert(SLH_MAX_K >= prm->k);
      432 :     base_2b(vi, md, prm->a, prm->k);
		  : 
      309 :     for (i = 0; i < prm->k; i++) {
		  : 
		  :         //  fors_SKgen()
      564 :         adrs_set_tree_index(ctx, (i << prm->a) + vi[i]);
      198 :         prm->fors_hash(ctx, sf, 0);
       66 :         sf += n;
		  : 
      891 :         for (j = 0; j < prm->a; j++) {
      594 :             s = (vi[i] >> j) ^ 1;
   130086 :             fors_node(  ctx, sf, (i << (prm->a - j)) + s, j);
      396 :             sf += n;
		  :         }
		  :     }
       12 :     return n * prm->k * (1 + prm->a);
       14 : }
		  : 
		  : //  === Compute a FORS public key from a FORS signature.
		  : //  Algorithm 16: fors_pkFromSig(SIGFORS , md, PK.seed, ADRS)
		  : 
		  : static void fors_pk_from_sig(   slh_ctx_t *ctx, uint8_t *pk,
		  :                                 const uint8_t *sf, const uint8_t *md)
       24 : {
		  : 
        3 :     const slh_param_t *prm = ctx->prm;
		  :     uint32_t i, j, idx;
		  :     uint32_t vi[SLH_MAX_K];
		  :     uint8_t root[SLH_MAX_K * SLH_MAX_N];
		  :     uint8_t *node;
        3 :     size_t  n = prm->n;
		  : 
     1284 :     base_2b(vi, md, prm->a, prm->k);
		  : 
        6 :     node = root;
      300 :     for (i = 0; i < prm->k; i++) {
		  : 
      198 :         adrs_set_tree_height(ctx, 0);
		  : 
      810 :         idx = (i << prm->a) + vi[i];
      999 :         adrs_set_tree_index(ctx, idx);
		  : 
      495 :         prm->h_f(ctx, node, sf);
       99 :         sf += n;
		  : 
     1485 :         for (j = 0; j < prm->a; j++) {
		  : 
     7128 :             adrs_set_tree_height(ctx, j + 1);
     6534 :             adrs_set_tree_index(ctx, idx >> (j + 1));
		  : 
     1782 :             if (((vi[i] >> j) & 1) == 0) {
     2970 :                 prm->h_h(ctx, node, node, sf);
		  :             } else {
     1794 :                 prm->h_h(ctx, node, sf, node);
		  :             }
      594 :             sf += n;
		  :         }
       99 :         node += n;
		  :     }
		  : 
       15 :     adrs_set_type_and_clear_not_kp(ctx, ADRS_FORS_ROOTS);
       45 :     prm->h_t(ctx, pk, root, prm->k * n);
       21 : }
		  : 
		  : //  === Public API
		  : 
		  : //  Return standard identifier string for parameter set *prm, or NULL.
		  : const char *slh_alg_id(const slh_param_t *prm)
		  : {
		  :     return prm->alg_id;
        8 : }
		  : 
		  : //  Return public (verification) key size in bytes for parameter set *prm.
		  : size_t slh_pk_sz(const slh_param_t *prm)
		  : {
		  :     return 2 * prm->n;
		  : }
		  : 
		  : //  Return private (signing) key size in bytes for parameter set *prm.
		  : size_t slh_sk_sz(const slh_param_t *prm)
		  : {
        1 :     return 4 * prm->n;
        2 : }
		  : 
		  : //  === Generate an SLH-DSA key pair.
		  : //  Algorithm 17: slh_keygen()
		  : 
		  : int slh_keygen(uint8_t *pk, uint8_t *sk,
		  :                int (*rbg)(uint8_t *x, size_t xlen), const slh_param_t *prm)
       10 : {
		  : 
		  :     slh_ctx_t   ctx;
		  :     uint8_t     pk_root[SLH_MAX_N];
        1 :     size_t      n = prm->n;
		  : 
        4 :     rbg(sk, 3 * n);                     //  SK.seed || SK.prf || PK.seed
        6 :     memcpy(pk, sk + 2 * n, n);          //  PK.seed
        6 :     memset(sk + 3 * n, 0x00, n);        //  PK.root not generated yet
        6 :     prm->mk_ctx(&ctx, NULL, sk, prm);   //  fill in partial
		  : 
        9 :     adrs_zero(&ctx);
       15 :     adrs_set_layer_address(&ctx, prm->d - 1);
        6 :     xmss_node(&ctx, pk_root, 0, prm->hp);
		  : 
		  :     //  fill pk_root
        5 :     memcpy(sk + 3 * n, pk_root, n);
        5 :     memcpy(pk + n, pk_root, n);
		  :     return 0;
        9 : }
		  : 
		  : //  === Generate an SLH-DSA signature.
		  : //  Algorithm 18: slh_sign(M, SK)
		  : 
		  : //  (Shared helper function for algorithms 18 and 19.)
		  : 
		  : static void split_digest(uint64_t *i_tree, uint32_t *i_leaf,
		  :                          const uint8_t *digest, const slh_param_t *prm)
		  : {
       45 :     size_t      md_sz       = (prm->k * prm->a + 7) / 8;
        3 :     const uint8_t *pi_tree  = digest + md_sz;
       15 :     size_t      i_tree_sz   = (prm->h - prm->hp + 7) / 8;
      261 :     *i_tree     = slh_toint(pi_tree, i_tree_sz);
        6 :     size_t      i_leaf_sz   = (prm->hp + 7) / 8;
        3 :     const uint8_t *pi_leaf  = pi_tree + i_tree_sz;
       27 :     *i_leaf     = slh_toint(pi_leaf, i_leaf_sz);
       15 :     if ((prm->h - prm->hp) != 64) {
       33 :         *i_tree     &= (UINT64_C(1) << (prm->h - prm->hp)) - UINT64_C(1);
		  :     }
       15 :     *i_leaf     &= (1 << prm->hp) - 1;
        3 : }
		  : 
		  : //  Core signing function that just takes in "digest" and an already
		  : //  initialized secret key context. *sig points to signature after randomizer.
		  : //  Returns the length of |SIG_FORS + SIG_HT| written at *sig.
		  : 
		  : size_t slh_do_sign( slh_ctx_t *ctx, uint8_t *sig, const uint8_t *digest)
       15 : {
		  :     const uint8_t   *md = digest;
		  :     uint64_t i_tree = 0;
		  :     uint32_t i_leaf = 0;
		  :     uint8_t pk_fors[SLH_MAX_N];
		  :     size_t sig_sz;
		  : 
        5 :     split_digest(&i_tree, &i_leaf, digest, ctx->prm);
		  : 
        9 :     adrs_zero(ctx);
       25 :     adrs_set_tree_address(ctx, i_tree);
        4 :     adrs_set_type_and_clear_not_kp(ctx, ADRS_FORS_TREE);
       11 :     adrs_set_key_pair_address(ctx, i_leaf);
		  : 
		  :     //  SIG_FORS
        8 :     sig_sz  = fors_sign(ctx, sig, md);
        6 :     fors_pk_from_sig(ctx, pk_fors, sig, md);
		  : 
		  :     //  SIG_HT
        2 :     sig +=  sig_sz;
     1363 :     sig_sz  += ht_sign(ctx, sig, pk_fors, i_tree, i_leaf);
		  : 
		  :     return sig_sz;
       15 : }
		  : 
		  : size_t slh_sign(uint8_t *sig, const uint8_t *m, size_t m_sz,
		  :                 const uint8_t *sk, int (*rbg)(uint8_t *x, size_t xlen),
		  :                 const slh_param_t *prm)
       13 : {
		  :     slh_ctx_t   ctx;
		  :     uint8_t opt_rand[SLH_MAX_N];
		  :     uint8_t digest[SLH_MAX_M];
		  : 
		  :     //  set up secret key etc
        6 :     prm->mk_ctx(&ctx, NULL, sk, prm);
		  : 
		  : #ifdef SLH_DETERMINISTIC
		  :     memcpy(opt_rand, ctx.pk_seed, prm->n);
		  : #else
        3 :     rbg(opt_rand, prm->n);
		  : #endif
		  : 
		  :     //  randomized hashing; R
		  :     uint8_t *r  = sig;
        1 :     size_t  sig_sz = prm->n;
        7 :     prm->prf_msg(&ctx, r, opt_rand, m, m_sz);
        7 :     prm->h_msg(&ctx, digest, r, m, m_sz);
		  : 
		  :     //  create FORS and HT signature parts
        5 :     sig_sz += slh_do_sign(&ctx, sig + sig_sz, digest);
		  : 
		  :     return sig_sz;
        9 : }
		  : 
		  : //  === Verify an SLH-DSA signature.
		  : //  Algorithm 19: slh_verify(M, SIG, PK)
		  : 
		  : bool slh_verify(const uint8_t *m, size_t m_sz,
		  :                 const uint8_t *sig, const uint8_t *pk,
		  :                 const slh_param_t *prm)
       32 : {
		  : 
		  :     slh_ctx_t   ctx;
		  :     uint8_t digest[SLH_MAX_M];
		  :     uint8_t pk_fors[SLH_MAX_N];
		  : 
		  :     const uint8_t   *r          = sig;
        4 :     const uint8_t   *sig_fors   = sig + prm->n;
       50 :     const uint8_t   *sig_ht     = sig + ((1 + prm->k*(1 + prm->a)) * prm->n);
		  : 
       12 :     prm->mk_ctx(&ctx, pk, NULL, prm);
       14 :     prm->h_msg(&ctx, digest, r, m, m_sz);
		  : 
		  :     const uint8_t   *md = digest;
		  :     uint64_t        i_tree = 0;
		  :     uint32_t        i_leaf = 0;
       14 :     split_digest(&i_tree, &i_leaf, digest, prm);
		  : 
       16 :     adrs_zero(&ctx);
       50 :     adrs_set_tree_address(&ctx, i_tree);
        8 :     adrs_set_type_and_clear_not_kp(&ctx, ADRS_FORS_TREE);
       22 :     adrs_set_key_pair_address(&ctx, i_leaf);
		  : 
       12 :     fors_pk_from_sig(&ctx, pk_fors, sig_fors, md);
		  : 
     2702 :     bool sig_ok = ht_verify(&ctx, pk_fors, sig_ht, i_tree, i_leaf);
		  :     return sig_ok;
       24 : }
		  : 
