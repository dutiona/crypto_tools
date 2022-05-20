#include <catch2/catch_all.hpp>
#include <fmt/core.h>

#include "crypto_tools/crypto_tools.hpp"

#include <string>

TEST_CASE("Compute prime number", "[is prime]")
{
  auto bit_length = 128u;
  auto nb_non_primes = 0u;
  auto nb_primes = 0u;
  auto i = 0;
  for (int i = 0; i < 100; ++i) {
    auto prime = generate_large_prime(bit_length);
    // fmt::print("i={}\n", i);
    if (prime) {
      if (is_prime(*prime)) {
        ++nb_primes;
        // fmt::print("<{}> prime!\n", prime->str());
      } else {
        // fmt::print("<{}> non-prime!\n", prime->str());
        ++nb_non_primes;
      }
    } else {
      fmt::print("no prime generated with the allocated try numbers!");
      ++nb_non_primes;
    }
  }
  REQUIRE(nb_primes >= 75u);  // 75%
  REQUIRE(nb_non_primes <= 25u);  // 25%
  // fmt::print("primes <{}>, non-primes <{}>\n", nb_primes, nb_non_primes);
}