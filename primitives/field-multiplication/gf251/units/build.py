#!/usr/bin/env sage -python

#########################################
####         Configuration           ####
#########################################

nb_tests_for_addition = 100
nb_tests_for_multiplication = 100
nb_tests_for_square = 100
nb_tests_for_inverse = 100
nb_tests_for_pow251_gf251to16 = 100
nb_tests_for_inv_gf251to16 = 100

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
q = 251
Fq = FiniteField(q)
R = PolynomialRing(Fq, names=('X',)); (X,) = R._first_ngens(1)
a = Fq(2); p = X**2 - a
Fqt2 = R.quotient_ring(R.ideal(p), names=('r2',)); (r2,) = Fqt2._first_ngens(1)
R = PolynomialRing(Fqt2, names=('X',)); (X,) = R._first_ngens(1)
a = r2 + Fq(1); p = X**2 - a
Fqt4 = R.quotient_ring(R.ideal(p), names=('r4',)); (r4,) = Fqt4._first_ngens(1)
R = PolynomialRing(Fqt4, names=('X',)); (X,) = R._first_ngens(1)
a = r2*r4 + Fq(1); p = X**3 - a
Fqt12 = R.quotient_ring(R.ideal(p), names=('r12',)); (r12,) = Fqt12._first_ngens(1)
R = PolynomialRing(Fqt12, names=('X',)); (X,) = R._first_ngens(1)

def to_numerical(t, depth):
    if depth == 0:
        return [to_integer_repr(t)]
    else:
        res = []
        for v in list(t):
            res += to_numerical(v, depth-1)
        return res

class GF251to12(Data):
    def __init__(self, value):
        assert value in Fqt12
        super().__init__(value)

    def to_string(self):
        return to_c_array(to_numerical(self.value, depth=3))

    @staticmethod
    def get_format():
        return ('uint8_t', 12)

class GF251to16Array(Data):
    def __init__(self, value):
        super().__init__(value)

    def to_string(self):
        return to_c_array([to_integer_repr(v) for v in self.value])

    @staticmethod
    def get_format():
        return ('uint8_t', 16)
        
# Test Case 1: Test Addition
series = factory.new_battery_of_tests('gf251to12_add',
    [('input1', GF251to12), ('input2', GF251to12), ('output', GF251to12)]
)
for _ in range(nb_tests_for_addition):
    v1 = Fqt12.random_element()
    v2 = Fqt12.random_element()
    series.append(GF251to12(v1), GF251to12(v2), GF251to12(v1+v2))

# Test Case 2: Test Substraction
series = factory.new_battery_of_tests('gf251to12_sub',
    [('input1', GF251to12), ('input2', GF251to12), ('output', GF251to12)]
)
for _ in range(nb_tests_for_addition):
    v1 = Fqt12.random_element()
    v2 = Fqt12.random_element()
    series.append(GF251to12(v1), GF251to12(v2), GF251to12(v1-v2))

# Test Case 3: Test Multiplication
series = factory.new_battery_of_tests('gf251to12_mul',
    [('input1', GF251to12), ('input2', GF251to12), ('output', GF251to12)]
)
for _ in range(nb_tests_for_multiplication):
    v1 = Fqt12.random_element()
    v2 = Fqt12.random_element()
    series.append(GF251to12(v1), GF251to12(v2), GF251to12(v1*v2))

# Test Case 4: Test Power 251
series = factory.new_battery_of_tests('gf251to12_pow251',
    [('input', GF251to12), ('output', GF251to12)]
)
for _ in range(nb_tests_for_multiplication):
    v = Fqt12.random_element()
    series.append(GF251to12(v), GF251to12(v**251))

# Test Case 5: Test Inverse
series = factory.new_battery_of_tests('gf251to12_inv',
    [('input', GF251to12), ('output', GF251to12)]
)
series.append(GF251to12(Fqt12(0)), GF251to12(Fqt12(0)))
for _ in range(nb_tests_for_inverse-1):
    v = 0
    while v == 0:
        v = Fqt12.random_element()
    series.append(GF251to12(v), GF251to12(1/v))

# Test Case 6: Test Power251 GF(251^16)
series = factory.new_battery_of_tests('gf251to16_pow251',
    [('input', GF251to16Array)]
)
for i in range(16):
    v = [Fq(0)]*16
    v[i] = 1
    series.append(GF251to16Array(v))
for _ in range(nb_tests_for_pow251_gf251to16):
    v = [Fq.random_element() for _ in range(16)]
    series.append(GF251to16Array(v))

# Test Case 7: Test Inverse GF(251^16)
series = factory.new_battery_of_tests('gf251to16_inv',
    [('input', GF251to16Array)]
)
for _ in range(nb_tests_for_inv_gf251to16):
    v = [Fq.random_element() for _ in range(16)]
    series.append(GF251to16Array(v))

# Build Tests
factory.write()
