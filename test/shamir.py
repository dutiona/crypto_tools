"""
The following Python implementation of Shamir's Secret Sharing is
released into the Public Domain under the terms of CC0 and OWFa:
https://creativecommons.org/publicdomain/zero/1.0/
http://www.openwebfoundation.org/legal/the-owf-1-0-agreements/owfa-1-0

See the bottom few lines for usage. Tested on Python 2 and 3.
"""

from __future__ import division
from __future__ import print_function

import random
import functools

# 12th Mersenne Prime
# (for this application we want a known prime number as close as
# possible to our security level; e.g.  desired security level of 128
# bits -- too large and all the ciphertext is large; too small and
# security is compromised)
_PRIME = 2 ** 127 - 1
# 13th Mersenne Prime is 2**521 - 1

_RINT = functools.partial(random.SystemRandom().randint, 0)


def _eval_at(poly, x, prime):
    """Evaluates polynomial (coefficient tuple) at x, used to generate a
    shamir pool in make_random_shares below.
    """
    accum = 0
    for coeff in reversed(poly):
        accum *= x
        accum += coeff
        accum %= prime
    return accum


def make_random_shares(secret, minimum, shares, prime=_PRIME):
    """
    Generates a random shamir pool for a given secret, returns share points.
    """
    if minimum > shares:
        raise ValueError("Pool secret would be irrecoverable.")
    poly = [secret] + [_RINT(prime - 1) for i in range(minimum - 1)]
    points = [(i, _eval_at(poly, i, prime))
              for i in range(1, shares + 1)]
    return points


def _extended_gcd(a, b):
    """
    Division in integers modulus p means finding the inverse of the
    denominator modulo p and then multiplying the numerator by this
    inverse (Note: inverse of A is B such that A*B % p == 1) this can
    be computed via extended Euclidean algorithm
    http://en.wikipedia.org/wiki/Modular_multiplicative_inverse#Computation
    """
    print("_extended_gcd:[a={}, b={}]".format(a, b))
    x = 0
    last_x = 1
    y = 1
    last_y = 0
    while b != 0:
        quot = a // b
        # print("[quot={}]".format(quot))
        # a, b = b, a % b
        tmp = a
        a = b
        b = tmp % b

        # x, last_x = last_x - quot * x, x
        tmp = x
        x = last_x - quot * tmp
        last_x = tmp

        # y, last_y = last_y - quot * y, y
        tmp = y
        y = last_y - quot * tmp
        last_y = tmp

        print("_extended_gcd:[a={}, b={}, quot={}, x={}, last_x={}, y={}, last_y={}]".format(
            a, b, quot, x, last_x, y, last_y))
    print("_extended_gcd:[last_x={}, last_y={}]".format(last_x, last_y))
    return last_x, last_y


def _divmod(num, den, p):
    """Compute num / den modulo prime p

    To explain what this means, the return value will be such that
    the following is true: den * _divmod(num, den, p) % p == num
    """
    print("_divmod:[num={}, den={}, p={}]".format(num, den, p))
    inv, _ = _extended_gcd(den, p)
    ret = num*inv
    print("_divmod:[num*inv={}]".format(ret))
    return ret


def _lagrange_interpolate(x, x_s, y_s, p):
    """
    Find the y-value for the given x, given n (x, y) points;
    k points will define a polynomial of up to kth order.
    """
    k = len(x_s)
    assert k == len(set(x_s)), "points must be distinct"

    def PI(vals):  # upper-case PI -- product of inputs
        accum = 1
        for v in vals:
            accum *= v
        return accum
    nums = []  # avoid inexact division
    dens = []
    for i in range(k):
        others = list(x_s)
        cur = others.pop(i)
        nums.append(PI(x - o for o in others))
        dens.append(PI(cur - o for o in others))
    den = PI(dens)
    num = sum([_divmod(nums[i] * den * y_s[i] % p, dens[i], p)
               for i in range(k)])
    return (_divmod(num, den, p) + p) % p


def recover_secret(shares, prime=_PRIME):
    """
    Recover the secret from share points
    (x, y points on the polynomial).
    """
    if len(shares) < 3:
        raise ValueError("need at least three shares")
    x_s, y_s = zip(*shares)
    return _lagrange_interpolate(0, x_s, y_s, prime)


def main():
    """Main function"""
    secret = 1234
    shares = make_random_shares(secret, minimum=3, shares=6)

    print('Secret:                                                     ',
          secret)
    print('Shares:')
    if shares:
        for share in shares:
            print('  ', share)

    print('Secret recovered from minimum subset of shares:             ',
          recover_secret(shares[:4]))
    print('Secret recovered from a different minimum subset of shares: ',
          recover_secret(shares[-4:]))


if __name__ == '__main__':
    #_extended_gcd(-6, 170141183460469231731687303715884105727)
    # _divmod(123153771853555882574203563594358837679, -
    #        6, 170141183460469231731687303715884105727)
    main()
