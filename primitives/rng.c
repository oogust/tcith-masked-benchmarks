#include "rng.h"

uint32_t get_random() { return 0;//rand(); 
}
uint8_t rand8()   { return (uint8_t) (get_random() & 0XFF); }
uint16_t rand16() { return (uint16_t) (get_random() & 0XFFFF); }
uint32_t rand32() { return get_random(); }
uint64_t rand64() { return (uint64_t) get_random() || (uint64_t) get_random() << 32 ; }