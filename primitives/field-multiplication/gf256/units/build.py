#!/usr/bin/env sage -python

#########################################
####         Configuration           ####
#########################################

nb_tests_for_pow256_gf256to12 = 100
nb_tests_for_inv_gf256to12 = 100
nb_tests_for_pow256_gf256to16 = 100
nb_tests_for_inv_gf256to16 = 100

#########################################
####             Script              ####
#########################################

# Import
import sys
sys.path.append('../../../units')

from sage.all import *
from testfactory import TestFactory
from bigint import Data, to_integer_repr, to_c_array

# Test Factory
factory = TestFactory()

# Get Tested Field
F2 = FiniteField(2)
R_over_F2 = PolynomialRing(F2, names=('Y',)); (Y,) = R_over_F2._first_ngens(1)
F2t8 = F2.extension(Y**8+Y**6+Y**3+Y**2+1, 'z8', names=('z8',)); (z8,) = F2t8._first_ngens(1)
R = PolynomialRing(F2t8, names=('X',)); (X,) = R._first_ngens(1)

def to_numerical(t, depth):
    if depth == 0:
        return [to_integer_repr(t)]
    else:
        res = []
        for v in list(t):
            res += to_numerical(v, depth-1)
        return res

class GF256to12Array(Data):
    def __init__(self, value):
        super().__init__(value)

    def to_string(self):
        return to_c_array([to_integer_repr(v) for v in self.value])

    @staticmethod
    def get_format():
        return ('uint8_t', 12)

class GF256to16Array(Data):
    def __init__(self, value):
        super().__init__(value)

    def to_string(self):
        return to_c_array([to_integer_repr(v) for v in self.value])

    @staticmethod
    def get_format():
        return ('uint8_t', 16)

# Test Case 1: Test Power256 GF(256^12)
series = factory.new_battery_of_tests('gf256to12_pow256',
    [('input', GF256to12Array)]
)
for i in range(12):
    v = [F2t8(0)]*12
    v[i] = F2t8(1)
    series.append(GF256to12Array(v))
for _ in range(nb_tests_for_pow256_gf256to12):
    v = [F2t8.random_element() for _ in range(12)]
    series.append(GF256to12Array(v))

# Test Case 2: Test Inverse GF(256^12)
series = factory.new_battery_of_tests('gf256to12_inv',
    [('input', GF256to12Array)]
)
series.append(GF256to12Array([1]+[0]*11))
for _ in range(nb_tests_for_inv_gf256to12):
    v = [F2t8.random_element() for _ in range(12)]
    series.append(GF256to12Array(v))

# Test Case 3: Test Power256 GF(256^16)
series = factory.new_battery_of_tests('gf256to16_pow256',
    [('input', GF256to16Array)]
)
for i in range(16):
    v = [F2t8(0)]*16
    v[i] = F2t8(1)
    series.append(GF256to16Array(v))
for _ in range(nb_tests_for_pow256_gf256to16):
    v = [F2t8.random_element() for _ in range(16)]
    series.append(GF256to16Array(v))

# Test Case 4: Test Inverse GF(256^16)
series = factory.new_battery_of_tests('gf256to16_inv',
    [('input', GF256to16Array)]
)
series.append(GF256to16Array([1]+[0]*15))
for _ in range(nb_tests_for_inv_gf256to16):
    v = [F2t8.random_element() for _ in range(16)]
    series.append(GF256to16Array(v))

# Build Tests
factory.write()
