CC=gcc-12
WARNING_FLAGS=-Wall -Wextra -Wpedantic -Wshadow
CFLAGS= -O3 -march=native $(WARNING_FLAGS) -std=gnu99

BENCH_INCLUDE=../../../app/bench

bench: bench.c ../gf251.c ../gf251-avx2-ct.c $(BENCH_INCLUDE)/timing.c
	$(CC) $(CFLAGS) bench.c ../gf251.c ../gf251-avx2-ct.c $(BENCH_INCLUDE)/timing.c -I.. -I$(BENCH_INCLUDE) -DNO_RND -o $@

## To clean
clean:
	rm -f bench
