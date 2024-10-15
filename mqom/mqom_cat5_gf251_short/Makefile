include ../common_compiler.mk

SYM_OBJ= rnd.o hash.o xof.o
ARITH_OBJ= gf251-matrix.o gf251.o
MPC_OBJ= mpc.o witness.o serialization-specific.o precomputed.o
CORE_OBJ= keygen.o sign.o views.o commit.o sign-mpcith-hypercube.o tree.o

APP_BENCH_OBJ=benchmark/bench.o benchmark/timing.o
APP_WRITE_BENCH_OBJ=benchmark/bench-write.o
APP_READ_BENCH_OBJ=benchmark/bench-read.o
APP_KAT_MAIN_OBJ=generator/PQCgenKAT_sign.o generator/rng.o
APP_KAT_CHECK_OBJ=generator/PQCgenKAT_check.o generator/rng.o

APP_INCLUDE=-Ibenchmark

include ../common.mk
