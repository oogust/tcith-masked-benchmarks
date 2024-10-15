from sage.all import *

n = 251
k0 = ceil(log(n,2))
overflow = 64

for k in range(k0,overflow):
    for v in range(0,2):
        m = floor(2**k / n) + v

        # avoid overflow
        maxi = floor((2**overflow-1) / m)

        # test
        for a in range(0, maxi+1):
            q = (a*m) // (2**k)
            r = a-n*q
            if r != (a % n):
                print(k, v, a-1, 'correctness', m)
                break
            elif a == maxi:
                print(k, v, a, 'overflow', m)


