#include "crypto_tools/prime.hpp"

#include <boost/multiprecision/miller_rabin.hpp>
#include <boost/random.hpp>
#include <fmt/core.h>

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>

/*
 * Calculate the number of trial divisions that gives the best speed in
 * combination with Miller-Rabin prime test, based on the sized of the prime.
 */
static constexpr unsigned calc_trial_divisions(unsigned bits)
{
  if (bits <= 512)
    return 64;
  else if (bits <= 1024)
    return 128;
  else if (bits <= 2048)
    return 384;
  else if (bits <= 4096)
    return 1024;
  return 2048;
}

// Function to return a^n
template<class Int>
static constexpr boost::multiprecision::cpp_int fast_pow2(Int&& n)
{
  boost::multiprecision::cpp_int one = 1;
  return (one << std::forward<Int>(n));
}

template<class Int>
static constexpr unsigned calc_bit_length(Int&& n)
{
  return boost::multiprecision::msb(std::forward<Int>(n)) + 1;
}
// This function is called for all k trials. It returns
// false if n is composite and returns true if n is
// probably prime.
// d is an odd number such that  d*2<sup>r</sup> = n-1
// for some r >= 1
bool is_prime(const boost::multiprecision::cpp_int& n)
{
  return boost::multiprecision::miller_rabin_test(
      n, calc_trial_divisions(calc_bit_length(n)));
}

static expected_unsigned_long_long generate_prime_candidate(unsigned bit_length,
                                                            bool safe)
{
  if (bit_length == 0)
    return tl::unexpected {generate_prime_error::bit_length_too_small};

  auto gen = std::mt19937_64 {std::random_device {}()};

  // translate bit length into an interval
  auto dist =
      boost::random::uniform_int_distribution<boost::multiprecision::cpp_int> {
          fast_pow2(bit_length - 1), fast_pow2(bit_length)};

  auto max_attempts = (safe ? 100 : 1) * 1000 * (std::log2(bit_length) + 1);

  for (unsigned i = 0; i < max_attempts; ++i) {
    boost::multiprecision::cpp_int n = dist(gen);
    // set low/high bytes to "remove" even numbers
    n |= (1ull << (bit_length - 1)) | 1;

    assert(calc_bit_length(n) == bit_length);

    if (is_prime(n)) {
      if (not safe)
        return n;

      if (safe && is_prime((n - 1) / 2)) {
        return n;
      }
    }
  }

  return tl::unexpected {generate_prime_error::failure_after_too_many_tries};
}

expected_unsigned_long_long generate_large_prime(unsigned bit_length, bool safe)
{
  return generate_prime_candidate(bit_length, safe);
}
