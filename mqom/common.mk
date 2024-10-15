HASH_PATH ?= ../sha3
HASH_MAKE_OPTIONS ?= PLATFORM=opt64

HASH_INCLUDE ?= -I$(HASH_PATH) -I../ -I./ -I$(HASH_PATH)/opt64

%.o : %.c
	$(CC) -c $(ALL_FLAGS) $(HASH_INCLUDE) $(APP_INCLUDE) -I. $< -o $@

all: kat_gen bench

libhash:
	$(HASH_MAKE_OPTIONS) make -C $(HASH_PATH)


# Library
LIB_OBJ = $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ)
LIB_SRC = $(LIB_OBJ:%.o=%.c)

libsign: $(LIB_OBJ) libhash
	ar rcs libsign.a $(LIB_OBJ)


# Generation of the KAT
sign: kat_gen
	cp kat_gen sign

kat_gen: $(APP_KAT_MAIN_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) libhash
	$(CC) $(APP_KAT_MAIN_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) $(ALL_FLAGS) -L$(HASH_PATH) -L. -lhash -lcrypto -o $@

kat_check: $(APP_KAT_CHECK_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) libhash
	$(CC) $(APP_KAT_CHECK_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) $(ALL_FLAGS) -L$(HASH_PATH) -L. -lhash -lcrypto -o $@

## Benchmark Tool

bench: $(APP_BENCH_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) libhash
	$(CC) $(APP_BENCH_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) $(ALL_FLAGS) -L$(HASH_PATH) -lhash -lm -L. -o $@

bench-write: $(APP_WRITE_BENCH_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) libhash
	$(CC) $(APP_WRITE_BENCH_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) $(ALL_FLAGS) -L$(HASH_PATH) -lhash -lm -L. -o $@

bench-read: $(APP_READ_BENCH_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) libhash
	$(CC) $(APP_READ_BENCH_OBJ) $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ) $(ALL_FLAGS) -L$(HASH_PATH) -lhash -lm -L. -o $@

# Cleaning

clean:
	rm -f $(SYM_OBJ) $(ARITH_OBJ) $(MPC_OBJ) $(CORE_OBJ)
	rm -f $(APP_BENCH_OBJ) $(APP_RAW_BENCH_OBJ)  $(APP_WRITE_BENCH_OBJ) $(APP_READ_BENCH_OBJ) $(APP_KAT_MAIN_OBJ)
	rm -rf unit-*
	rm -f bench bench-*
	rm -f kat_gen
	$(HASH_MAKE_OPTIONS) make -C $(HASH_PATH) clean
	find . -name "*.o" -type f -delete
	find . -name "*.a" -type f -delete
