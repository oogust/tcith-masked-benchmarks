#ifndef __SLOTH_DRV__
#define __SLOTH_DRV__

#include "sloth_map.h"
#include "sloth_hal.h"
#include <string.h>

#ifndef hexdump
#define hexdump(v, len, prefix) do {\
    unsigned int _i_;\
    if(prefix != NULL){\
        printf("%s: ", prefix);\
    }\
    for(_i_ = 0; _i_ < len; _i_++){\
        printf("%02x", (v)[_i_]);\
    }\
    printf("\n");\
} while(0);
#endif

// Keccak-f[1600] permutation call in hardware, unmasked
static inline void crx_hw_keccak_f1600(void *v)
{
    uint32_t *v32 = (uint32_t *) v;
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
    int i;

    for (i = 0; i < 50; i++) {              //  actually slow part 1
        r32[i] = v32[i];
    }
    r32[KECC_STOP] = 0x74;                  //  stop position (default)
    r32[KECC_TRIG] = 0x01;                  //  start it
    KECC_WAIT

    for (i = 0; i < 50; i++) {              //  actually slow part 2
        v32[i] = r32[i];
    }
}

// Keccak-f[1600] permutation call in hardware, masked at order 2
// (v ^ m1 ^ m2 = real input / output)
// The caller is expected to mask and unmask
static inline void crx_hw_keccak_f1600_m(void *v, void *m1, void *m2)
{
    uint32_t *v32 = (uint32_t *) v;
    volatile uint32_t   *r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;
    int i;

    for (i = 0; i < 50; i++) {              //  actually slow part 1
        r32[i] = v32[i];
        if(m1){
           r32[KTI3_MEMB + i] = ((uint32_t *)m1)[i];
        }
        if(m2){
           r32[KTI3_MEMC + i] = ((uint32_t *)m2)[i];
        }
    }
    r32[KTI3_STOP] = 0x74;                  //  stop position (default)
    r32[KTI3_TRIG] = 0x01;                  //  start it
    KTI3_WAIT

    for (i = 0; i < 50; i++) {              //  actually slow part 2
        v32[i] = r32[i];
        if(m1){
            ((uint32_t *)m1)[i] = r32[KTI3_MEMB + i];
        }
        if(m2){
            ((uint32_t *)m2)[i] = r32[KTI3_MEMC + i];
        }
    }
}


// ==================================================================================
// ==================================================================================
//
// The following structures track the state of the accelerators to optimize copies
// in the inner state
static volatile void *KECC_LAST_CTX   = NULL;
static volatile void *KECTI3_LAST_CTX = NULL;
//static volatile void *SHA256_LAST_CTX = NULL;
//static volatile void *SHA512_LAST_CTX = NULL;

// This is the SHA-3 XKCP library API to plug into high level
typedef enum { KECCAK_SUCCESS = 0, KECCAK_FAIL = 1, KECCAK_BAD_HASHLEN = 2 } HashReturn;
typedef unsigned char BitSequence;
typedef size_t BitLength;

typedef struct {
    uint8_t state[200];
    unsigned int block_size;
    unsigned int byte_idx;
    unsigned int rate;
    unsigned int capacity;
    unsigned int outlen;
    unsigned char suffix;
} __attribute__((aligned(4))) Keccak_HashInstance;

static inline HashReturn Keccak_HashInitialize(Keccak_HashInstance *hashInstance, unsigned int rate, unsigned int capacity, unsigned int hashbitlen, unsigned char delimitedSuffix)
{
    // Set the rate, capacity, block size, ...
    hashInstance->rate = rate;
    hashInstance->capacity = capacity;
    hashInstance->outlen = hashbitlen / 8;
    hashInstance->suffix = delimitedSuffix;
    hashInstance->byte_idx = 0;
    hashInstance->block_size = rate / 8;

    volatile uint32_t   *r32 = NULL;
    // If both blocks are taken, release one
    if((KECC_LAST_CTX != NULL) && (KECTI3_LAST_CTX != NULL)){
        r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
        memcpy(((Keccak_HashInstance*)KECC_LAST_CTX)->state, (void *)r32, 200);
        KECC_LAST_CTX = NULL;
    }
    
    // Initialize the inner context of a free hardware block
    if(KECC_LAST_CTX == NULL){
        r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
	KECC_LAST_CTX = hashInstance;
    }
    else{
        r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;
	KECTI3_LAST_CTX = hashInstance;
    }
    // Zeroize the state
    memset((void *)&r32[0], 0, 200);
    
    return KECCAK_SUCCESS;
}

/** Macro to initialize a SHAKE128 instance as specified in the FIPS 202 standard.
  */
#define Keccak_HashInitialize_SHAKE128(hashInstance)        Keccak_HashInitialize(hashInstance, 1344,  256,   0, 0x1F)

/** Macro to initialize a SHAKE256 instance as specified in the FIPS 202 standard.
  */
#define Keccak_HashInitialize_SHAKE256(hashInstance)        Keccak_HashInitialize(hashInstance, 1088,  512,   0, 0x1F)

/** Macro to initialize a SHA3-224 instance as specified in the FIPS 202 standard.
  */
#define Keccak_HashInitialize_SHA3_224(hashInstance)        Keccak_HashInitialize(hashInstance, 1152,  448, 224, 0x06)

/** Macro to initialize a SHA3-256 instance as specified in the FIPS 202 standard.
  */
#define Keccak_HashInitialize_SHA3_256(hashInstance)        Keccak_HashInitialize(hashInstance, 1088,  512, 256, 0x06)

/** Macro to initialize a SHA3-384 instance as specified in the FIPS 202 standard.
  */
#define Keccak_HashInitialize_SHA3_384(hashInstance)        Keccak_HashInitialize(hashInstance,  832,  768, 384, 0x06)

/** Macro to initialize a SHA3-512 instance as specified in the FIPS 202 standard.
  */
#define Keccak_HashInitialize_SHA3_512(hashInstance)        Keccak_HashInitialize(hashInstance,  576, 1024, 512, 0x06)

/**
  * Function to give input data to be absorbed.
  * @param  hashInstance    Pointer to the hash instance initialized by Keccak_HashInitialize().
  * @param  data        Pointer to the input data.
  *                     When @a databitLen is not a multiple of 8, the last bits of data must be
  *                     in the least significant bits of the last byte (little-endian convention).
  * @param  databitLen  The number of input bits provided in the input data.
  * @pre    In the previous call to Keccak_HashUpdate(), databitlen was a multiple of 8.
  * @return KECCAK_SUCCESS if successful, KECCAK_FAIL otherwise.
  */
static inline HashReturn Keccak_HashUpdate(Keccak_HashInstance *hashInstance, const BitSequence *data, BitLength databitlen)
{
    
    volatile uint32_t   *r32 = NULL;
    unsigned int i;

    // Check if our context is already used, if yes the state is already in place
    if(KECC_LAST_CTX == hashInstance){
        r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
    }
    else if(KECTI3_LAST_CTX == hashInstance){
        r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;
    }
    else{
        // No unit is already used, take one and copy the state after saving it in the old context
        r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
        memcpy(((Keccak_HashInstance*)KECC_LAST_CTX)->state, (void *)r32, 200);
        memcpy((void*)r32, hashInstance->state, 200);
        KECC_LAST_CTX = hashInstance;        
    }
  
    uint32_t byte_idx = hashInstance->byte_idx;
    uint32_t block_size = hashInstance->block_size;
    // XXX: TODO: try to optimize with 32 bits full blocks 
    for(i = 0; i < (databitlen / 8); i++){
        ((volatile uint8_t*)r32)[byte_idx++] ^= data[i];
        if(byte_idx == block_size){
            // Reset index
            byte_idx = 0;
            // Launch permutation
            if(r32 == (volatile uint32_t *) KECCAK_BASE_ADDR){
                r32[KECC_TRIG] = 0x01;
                KECC_WAIT
            }
            else{
                r32[KTI3_TRIG] = 0x01;
                KTI3_WAIT
            }
        }
    }
    hashInstance->byte_idx = byte_idx;

    return KECCAK_SUCCESS;
}

/**
  * Function to call after all input blocks have been input and to get
  * output bits if the length was specified when calling Keccak_HashInitialize().
  * @param  hashInstance    Pointer to the hash instance initialized by Keccak_HashInitialize().
  * If @a hashbitlen was not 0 in the call to Keccak_HashInitialize(), the number of
  *     output bits is equal to @a hashbitlen.
  * If @a hashbitlen was 0 in the call to Keccak_HashInitialize(), the output bits
  *     must be extracted using the Keccak_HashSqueeze() function.
  * @param  hashval     Pointer to the buffer where to store the output data.
  * @return KECCAK_SUCCESS if successful, KECCAK_FAIL otherwise.
  */
static inline HashReturn Keccak_HashFinal(Keccak_HashInstance *hashInstance, BitSequence *hashval)
{
    volatile uint32_t   *r32 = NULL;

    // Check if our context is already used, if yes the state is in the hardware block
    if(KECC_LAST_CTX == hashInstance){
        r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
    }
    else if(KECTI3_LAST_CTX == hashInstance){
        r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;
    }
    else{
        // No unit is already used, take one and copy the state after saving it in the old context
        r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
        memcpy(((Keccak_HashInstance*)KECC_LAST_CTX)->state, (void *)r32, 200);
        memcpy((void *)r32, hashInstance->state, 200);
        KECC_LAST_CTX = hashInstance;        
    }

    // Proceed with the padding of the last block
    ((volatile uint8_t*)r32)[hashInstance->byte_idx]       ^= hashInstance->suffix;
    ((volatile uint8_t*)r32)[hashInstance->block_size - 1] ^= 0x80;
    hashInstance->byte_idx = 0;

    // Call the permutation
    if(r32 == (volatile uint32_t *) KECCAK_BASE_ADDR){
        r32[KECC_TRIG] = 0x01;
        KECC_WAIT
    }
    else{
        r32[KTI3_TRIG] = 0x01;
        KTI3_WAIT
    }

    // Copy the hash output if needed
    if(hashInstance->outlen){
        memcpy(hashval, (void *)r32, hashInstance->outlen);
        // Release the hardware unit as we don't need it anymore
        if(KECC_LAST_CTX == hashInstance){
            KECC_LAST_CTX = NULL;
        }
        else{
            KECTI3_LAST_CTX = NULL;
        }
    }

    return KECCAK_SUCCESS;
}

 /**
  * Function to squeeze output data.
  * @param  hashInstance    Pointer to the hash instance initialized by Keccak_HashInitialize().
  * @param  data        Pointer to the buffer where to store the output data.
  * @param  databitlen  The number of output bits desired (must be a multiple of 8).
  * @pre    Keccak_HashFinal() must have been already called.
  * @pre    @a databitlen is a multiple of 8.
  * @return KECCAK_SUCCESS if successful, KECCAK_FAIL otherwise.
  */
static inline HashReturn Keccak_HashSqueeze(Keccak_HashInstance *hashInstance, BitSequence *data, BitLength databitlen)
{
    volatile uint32_t   *r32 = NULL;
    unsigned int i, idx, remain, sz, block_size;

    // Check if our context is already used, if yes the state is already in place
    if(KECC_LAST_CTX == hashInstance){
        r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
    }
    else if(KECTI3_LAST_CTX == hashInstance){
        r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;
    }
    else{
        // No unit is already used, take one and copy the state after saving it in the old context
        r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
        memcpy(((Keccak_HashInstance*)KECC_LAST_CTX)->state, (void *)r32, 200);
        memcpy((void*)r32, hashInstance->state, 200);
        KECC_LAST_CTX = hashInstance;        
    }

    block_size = hashInstance->block_size;
    i = 0;
    remain = (databitlen / 8);
    // Copy as much as remaining data
    idx = hashInstance->byte_idx;
    sz = (block_size - idx);
    sz = (remain < sz) ? remain : sz;
    if(sz != 0){
        memcpy(&data[0], &((uint8_t *)r32)[idx], sz);
    }
    remain -= sz;
    i += sz;

    // Squeeze as much time as needed
    while(remain){
        idx = 0;
        // Call the permutation
        if(r32 == (volatile uint32_t *) KECCAK_BASE_ADDR){
            r32[KECC_TRIG] = 0x01;
            KECC_WAIT
        }
        else{
            r32[KTI3_TRIG] = 0x01;
            KTI3_WAIT
        }
        sz = (remain < block_size) ? remain : block_size;
        memcpy(&data[i], (void *)r32, sz);
        i += sz;
        remain -= sz;
    }

    hashInstance->byte_idx = ((idx + sz) % block_size);

    return KECCAK_SUCCESS;
}

#endif
