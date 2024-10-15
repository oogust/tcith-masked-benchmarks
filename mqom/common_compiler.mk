CC?=gcc
ALL_FLAGS?=-O3 -flto -fPIC -std=c11 -march=native -Wall -Wextra -Wpedantic -Wshadow -DPARAM_HYPERCUBE_7R -DPARAM_GF251 -DPARAM_L1 -DNDEBUG
ALL_FLAGS+=$(EXTRA_ALL_FLAGS)
