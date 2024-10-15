#include "masksONOFF.h"
//#include <libopencm3/stm32/rng.h>

// enable/disable randomness for masks ON/OFF
volatile uint8_t en_rand = 1;

/**
 * Wrapper over rng_get_random_blocking,
 * allows masks ON/OFF through en_rand.
 */
uint32_t random_uint32_()
{
    return rand();
}



