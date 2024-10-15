# We are working in fields of characteristic 2
# We will build a tower of field extension
#   using the Theorems 3.75 and 3.79 of "Finite Field"
#   of Lidl and Niederreiter.
# ==== Theorem 3.79 ====
# Let 'a' belongs to F_q and p the characteristic of F_q
# Then, the trinomial x^p - x - a is irreducible in F_q[X]
# if and only if the trace Tr_{F_q}(a) is not zero.
#   => In our case:
# Let 'a' belongs to F_{2^m}
# Then, the trinomial x^2 - x - a is irreducible in F_{2^m}[X]
# if and only if the trace Tr_{F_{2^m}}(a) is not zero.
# ==== Theorem 3.75 ====
# Let 'a' belongs to F_q^*
# Then, the binomial x^t - a is irreducible in F_q[X]
# if and only if
#   (1) each prime factor of t divides the order e of a in F_q^*, but not (q-1)/e
#   (2) q % 4 = 1 if t % 4 = 0
#   => In our case:
# t = 3, so (2) is always satisfied. (1) can also be simplify into
#   (1') 3 should not divide (q-1)/e where e is the order of a in F_q^*,
#         or equivalently, a^((q-1)/3) should not be equal to 1.

def is_valid(q_pow, a, div=2):
    return a**((q_pow-1)//div) != 1

def from_integer_representation(F, v):
    r = F.gen()
    binary = list(map(int, bin(v)[2:].rjust(F.degree(), '0')))
    binary.reverse()
    res = F(0)
    for i, bit in enumerate(binary):
        if bit:
            res += (r**i)
    return res

def to_numerical(t):
    try:
        return t.integer_representation()
    except AttributeError:
        return [to_numerical(v) for v in list(t)]

def trace(v):
    F = v.parent()
    q = F.characteristic()
    m = round(log(F.order(),q))
    return sum(
        v**(q**(i))
        for i in range(m)
    )

# First step of the Tower
F2 = FiniteField(2)
R_over_F2.<Y> = PolynomialRing(F2)
#F2t8.<z8> = F2.extension(Y^8+Y^6+Y^3+Y^2+1, 'z8')
F2t8.<z8> = F2.extension(Y^8+Y^4+Y^3+Y+1, 'z8')
R.<X> = PolynomialRing(F2t8)

# Step 2
a = z8^5
p = X^2 + X + a
assert a.trace() != 0
assert p.is_irreducible()

coeffs = [coeff.integer_representation() for coeff in list(p)]
print(f" -> {coeffs}")

F2t16.<r16> = R.quotient_ring(R.ideal(p))
R.<X> = PolynomialRing(F2t16)

# Step 3
a = z8^5 * r16
p = X^2 + X + a
assert trace(a) != 0
#assert p.is_irreducible()

F2t32.<r32> = R.quotient_ring(R.ideal(p))
R.<X> = PolynomialRing(F2t32)

if False:
    a = z8^5 * r16 * r32
    p = X^2 + X + a
    assert trace(a) != 0

    F2t64.<r64> = R.quotient_ring(R.ideal(p))
    R.<X> = PolynomialRing(F2t64)

    for _ in range(10):
        t1 = F2t64.random_element()
        t2 = F2t64.random_element()
        res = t1*t2
        print(to_numerical(t1), to_numerical(t2), to_numerical(res))

else:
    a = z8^4 * r16 * r32
    p = X^3 - a
    assert is_valid(2**32, a, 3)
    print('Good')

    F2t96.<r96> = R.quotient_ring(R.ideal(p))
    R.<X> = PolynomialRing(F2t96)
