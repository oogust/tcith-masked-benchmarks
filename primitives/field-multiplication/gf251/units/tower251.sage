# We are working in fields of characteristic 251
# We will build a tower of field extension
#   using the Theorem 3.75 of "Finite Field"
#   of Lidl and Niederreiter:
# Let 'a' belongs to F_q^*
# Then, the binomial x^t - a is irreducible in F_q[X]
# if and only if
#   (1) each prime factor of t divides the order e of a in F_q^*, but not (q-1)/e
#   (2) q % 4 = 1 if t % 4 = 0
#   => In our case:
# t = 2, so (2) is always satisfied. (1) can also be simplify into
#   (1') 2 should not divide (q-1)/e where e is the order of a in F_q^*,
#         or equivalently, a^((q-1)/2) should not be equal to 1.
# t = 3, so (2) is always satisfied. (1) can also be simplify into
#   (1') 3 should not divide (q-1)/e where e is the order of a in F_q^*,
#         or equivalently, a^((q-1)/3) should not be equal to 1.

q  = 251
t  = 2
assert (q % 4 == 1) or not (t % 4 == 0)

def is_valid(q_pow, a, div=2):
    return a**((q_pow-1)//div) != 1

def from_integer_representation(F, v):
    r = F.primitive_element()
    binary = list(map(int, bin(v)[2:].rjust(F.degree(), '0')))
    binary.reverse()
    res = F(0)
    for i, bit in enumerate(binary):
        if bit:
            res += (r**i)
    return res

def to_numerical(t, is_hex=False):
    try:
        #return t.integer_representation()
        int(t) # <- To Test with Try-Except
        if is_hex:
            return hex(t)[2:]
        else:
            return int(t)
    #except AttributeError:
    except TypeError:
        return [to_numerical(v, is_hex=is_hex) for v in list(t)]

def trace(v):
    F = v.parent()
    q = F.characteristic()
    m = round(log(F.order(),q))
    return sum(
        v**(q**(i))
        for i in range(m)
    )

# First step of the Tower
Fq = FiniteField(q)
R.<X> = PolynomialRing(Fq)

# Step 2
a = Fq(2)
p = X^t - a
e = a.multiplicative_order()
#print(e, (q-1)/e)
assert (e % 2 == 0) and ((q-1)/e %2 != 0)
assert p.is_irreducible(), p.factor()
assert is_valid(q, a)

coeffs = to_numerical(p)
print(f" -> {coeffs}")

Fqt2.<r2> = R.quotient_ring(R.ideal(p))
R.<X> = PolynomialRing(Fqt2)

# Step 3
a = r2 + Fq(1)
p = X^t - a

def is_in_Fq(a):
    return a**q == a

def multiplicative_order(a):
    i = 1
    while True:
        if is_in_Fq(a**i):
            return Fq(int(str(a**i))).multiplicative_order()*i
        i += 1

def search():
    for i in range(1, 251):
        for j in range(251):
            a = i*r2 + Fq(j)
            if multiplicative_order(a) % 8 == 0:
                print(i, a, multiplicative_order(a))

e = multiplicative_order(a)
#print(e, (q^2-1)/e)
#assert p.is_irreducible()
assert is_valid(q**2, a)
coeffs = to_numerical(p)
print(f" -> {coeffs}")

Fqt4.<r4> = R.quotient_ring(R.ideal(p))
R.<X> = PolynomialRing(Fqt4)

#Step 4
if False:
    a = r2*r4 + Fq(1)
    p = X^2 - a
    assert is_valid(q**4, a)

    coeffs = to_numerical(p)
    print(f" -> {coeffs}")

    Fqt8.<r8> = R.quotient_ring(R.ideal(p))
    R.<X> = PolynomialRing(Fqt8)

    if True:
        a = (
            (45*r2+90)*r4 + (0*r2+1)
        )*r8 + (
            (100*r2+34)*r4 + (245*r2+250)
        )
        b = (
            (45*r2+90)*r4 + (0*r2+1)
        )*r8 + (
            (100*r2+34)*r4 + (245*r2+250)
        )

        a = Fqt8.random_element()
        b = Fqt8.random_element()

    print(to_numerical(a, True))
    print(to_numerical(b, True))
    print(to_numerical(a*b, True))

else:
    a = r2*r4 + Fq(1)
    p = X^3 - a
    assert is_valid(q**4, a, div=3)

    coeffs = to_numerical(p)
    print(f" -> {coeffs}")

    Fqt12.<r12> = R.quotient_ring(R.ideal(p))
    R.<X> = PolynomialRing(Fqt12)

    ## To build linear map of pow251
    for i in range(12):
        b = [0]*i + [1] + [0]*(12-1-i)
        #print(b)
        a = (
            (b[11]*r2+b[10])*r4 + (b[9]*r2+b[8])
        )*r12**2 + (
            (b[7]*r2+b[6])*r4 + (b[5]*r2+b[4])
        )*r12 + (
            (b[3]*r2+b[2])*r4 + (b[1]*r2+b[0])
        )
        print(to_numerical(a**251))

