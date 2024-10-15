		  : //  main.c
		  : //  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.
		  : 
		  : //  === testing main()
		  : 
		  : #include <string.h>
		  : #include "sloth_hal.h"
		  : #include "sloth_crx_drv.h"
		  : 
		  : const char main_hello[] =
		  : "\n[RESET]"
		  : "\t   ______        __  __ __\n"
		  : "\t  / __/ /  ___  / /_/ // /  SLotH Accelerator Test 2024/05\n"
		  : "\t _\\ \\/ /__/ _ \\/ __/ _  /   SLH-DSA / FIPS 205 ipd\n"
		  : "\t/___/____/\\___/\\__/_//_/    markku-juhani.saarinen@tuni.fi\n\n";
		  : 
		  : //  unit tests
		  : int test_sloth();       //  test_sloth.c
		  : int test_bench();       //  test_bench.c
		  : int test_leak();        //  test_leak.c
		  : 
		  : #ifdef _PICOLIBC__
		  : // XXX In case of Picolibc, redirect stdio related stuff to uart
		  : // (see https://github.com/picolibc/picolibc/blob/main/doc/os.md)
		  : // This allows to use printf family of functions
		  : #include <stdio.h>
		  : #include <stdlib.h>
		  : static int sample_putc(char c, FILE *file)
		  : {
		  :         (void) file;            /* Not used in this function */
		  :         sio_putc(c);            /* Defined by underlying system */
		  :         return c;
		  : }
		  : 
		  : static int sample_getc(FILE *file)
		  : {
		  :         unsigned char c;
		  :         (void) file;            /* Not used in this function */
		  :         c = sio_getc();         /* Defined by underlying system */
		  :         return c;
		  : }
		  : 
		  : FILE __stdio = FDEV_SETUP_STREAM(sample_putc,
		  :                                         sample_getc,
		  :                                         NULL,
		  :                                         _FDEV_SETUP_RW);
		  : 
		  : FILE *const stdin = &__stdio; __strong_reference(stdin, stdout); __strong_reference(stdin, stderr);
		  : #endif
		  : 
		  : static inline void hexdump(unsigned char *v, unsigned int len, const char *prefix)
		  : {
		  :     unsigned int i;
		  : 
		  :     if(prefix != NULL){
		  :         printf("%s: ", prefix);
		  :     }
		  :     for(i = 0; i < len; i++){
		  :         printf("%02x", v[i]);
		  :     }
		  :     printf("\n");
		  : }
		  : 
		  : #if 0
		  : int main()
		  : {
		  :     uint32_t start, stop;
		  : 
		  :     printf("[+] CRX SLotH tests\n");
		  : 
		  : #if 0
		  :     uint8_t m[200] = { 0 };
		  :     memcpy(m, "abc\x06", 4);              //  pad: 0x1F=SHAKE, 0x06=SHA-3
		  :     m[200 - 2*32 - 1] = 0x80;             //  rate/capacity for 256
		  : 
		  : 
		  :     hexdump(m, 200, "m1");
		  : 
		  :     crx_hw_keccak_f1600_m(&m, NULL, NULL);
		  : 
		  :     hexdump(m, 200, "m2");
		  : #endif
		  :     uint8_t digest1[32] = { 0 };
		  :     Keccak_HashInstance ctx1;
		  : 
		  :     start = get_clk_ticks();
		  :     Keccak_HashInitialize_SHA3_256(&ctx1);
		  :     stop = get_clk_ticks();
		  :     printf(" ==> Keccak_HashInitialize_SHA3_256 %ld:\n", stop-start);
		  : 
		  :     start = get_clk_ticks();
		  :     Keccak_HashUpdate(&ctx1, "abc", 3*8);
		  :     stop = get_clk_ticks();
		  :     printf(" ==> Keccak_HashUpdate: %ld\n", stop-start);
		  : 
		  :     start = get_clk_ticks();
		  :     Keccak_HashFinal(&ctx1, &digest1);
		  :     stop = get_clk_ticks();
		  :     printf(" ==> Keccak_HashFinal: %ld\n", stop-start);
		  : 
		  :     exit(0);
		  : }
		  : #else
		  : int main()
       10 : {
		  :     int fail = 0;
		  : 
        4 :     sio_puts(main_hello);
		  : 
        4 :     sio_puts("[INFO]\t=== Basic health test ===\n");
        3 :     fail += test_sloth();
        4 :     sio_puts("\n[INFO]\t=== Testbench === \n");
        3 :     fail += test_bench();
		  :     //fail += test_leak();
		  : 
        1 :     if (fail) {
		  :         sio_puts("[FAIL]\tSome tests failed.\n");
		  :     } else {
        5 :         sio_puts("[PASS]\tAll tests ok.\n");
		  :     }
		  : 
		  :     //  get input (test UART)
		  : #ifdef SLOTH
        4 :     sio_puts("\nUART Test. Press x to exit.\n");
		  :     int ch, gpio, old_gpio;
		  : 
		  :     ch = 0;
        2 :     old_gpio = -1;
		  : 
		  :     do {
        3 :         gpio = get_gpio_in();
        1 :         if (gpio != old_gpio) {
        4 :             sio_puts("GPIO 0x");
        4 :             sio_put_hex(gpio, 2);
        3 :             sio_putc('\n');
		  :             old_gpio = gpio;
		  :         }
		  : 
        4 :         if (get_uart_rxok()) {
        4 :             ch = get_uart_rx();
        3 :             sio_puts("UART 0x");
        4 :             sio_put_hex(ch, 2);
        3 :             sio_putc(' ');
        3 :             sio_putc(ch);
        3 :             sio_putc('\n');
		  :         }
		  : 
        2 :     } while (ch != 'x');
		  : #endif
        3 :     sio_putc('\n');
        3 :     sio_putc(4);  //  translated to EOF
        3 :     sio_putc(0);
		  : 
		  : #ifdef _PICOLIBC__
		  :     // XXX: in case of picolibc, explicitly exit as
		  :     // this is not performed at the return of main
        3 :     exit(0);
		  : #endif
		  :     return 0;
		  : }
		  : #endif
