#include <array>
#include <cmath>
#include <random>

#include "crypto_tools/crypto_tools.hpp"

static auto& get_seeded_random_device()
{
  static std::random_device rd;
  static std::mt19937_64 rd_mt(rd());
  return rd_mt;
}

static unsigned long long mod_pow(unsigned long long num,
                                  unsigned long long pow,
                                  unsigned long long mod)
{
  unsigned long long test;
  unsigned long long n = num;
  for (test = 1; pow; pow >>= 1) {
    if (pow & 1)
      test = ((test % mod) * (n % mod)) % mod;
    n = ((n % mod) * (n % mod)) % mod;
  }

  return test;
}

// Function to return a^n
static unsigned long long fast_pow(unsigned long long a, unsigned long long n)
{
  // Stores final answer
  unsigned long long ans = 1;

  while (n > 0) {
    unsigned long long last_bit = (n & 1);

    // Check if current LSB is set
    if (last_bit) {
      ans = ans * a;
    }

    a = a * a;

    // Right shift
    n = n >> 1;
  }

  return ans;
}

static bool Rabin_Miller(unsigned long long n)
{
  auto s = n - 1;
  auto t = 0ull;
  while (s & 1 == 0) {
    s = s / 2;
    t += 1;
  }
  auto k = 0ull;
  auto dist = std::uniform_int_distribution<unsigned long long> {2, n - 1};
  while (k < 128) {
    auto a = dist(get_seeded_random_device());
    // a^s is computationally infeasible.  we need a more intelligent
    // approach v = (a**s)%n python's core math module can do modular
    // exponentiation
    auto v = mod_pow(a, s, n);  // where values are(num, exp, mod)
    if (v != 1) {
      auto i = 0ull;
      while (v != (n - 1)) {
        if (i == t - 1) {
          return false;
        } else {
          ++i;
          v = (v * v) % n;
        }
      }
    }
    k += 2;
  }
  return true;
}

bool is_prime(unsigned long long n)
{
  // lowPrimes is all primes (sans 2, which is covered by the bitwise and
  // operator) under 1000. taking n modulo each lowPrime allows us to remove a
  // huge chunk of composite numbers from our potential pool without resorting
  // to Rabin-Miller
  static constexpr unsigned long long low_primes[] = {
      3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37,  41,  43,  47,
      53,  59,  61,  67,  71,  73,  79,  83,  89,  97,  101, 103, 107, 109,
      113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191,
      193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269,
      271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353,
      359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439,
      443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523,
      541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617,
      619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709,
      719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811,
      821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907,
      911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997};

  static constexpr auto low_primes_arr = std::to_array(low_primes);

  if (n >= 3) {
    if (n & 1 != 0) {
      for (auto p : low_primes_arr) {
        if (n == p) {
          return true;
        }
        if (n % p == 0) {
          return false;
        }
      }
      return Rabin_Miller(n);
    }
  }
  return false;
}

expected_unsigned_long_long generate_large_prime(unsigned long long k)
{
  auto r = 100 * (std::log2(k) + 1);  // number of attempts max
  auto r_ = r;
  auto dist = std::uniform_int_distribution<unsigned long long> {
      fast_pow(2, k - 1), fast_pow(2, k)};
  while (r > 0) {
    // randrange is mersenne twister and is completely deterministic
    // unusable for serious crypto purposes
    auto n = dist(get_seeded_random_device());
    r -= 1;
    if (is_prime(n)) {
      return n;
    }
  }
  return tl::unexpected {generate_error::failure_after_too_many_tries};
}
