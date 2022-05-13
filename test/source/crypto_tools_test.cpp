#include <string>

#include "crypto_tools/crypto_tools.hpp"

#include <catch2/catch_all.hpp>
#include <fmt/core.h>

TEST_CASE("Compute prime number", "[is prime]")
{
  auto bit_length = 64;
  auto nb_non_prime = 0ull;
  auto nb_prime = 0ull;
  auto i = 0;
  while (i < 100) {
    auto prime = generate_large_prime(bit_length);
    fmt::print("i={}\n", i);
    if (prime) {
      if (is_prime(*prime, bit_length))
        ++nb_prime;
      else
        ++nb_non_prime;
    }
    ++i;
  }
  REQUIRE(nb_prime >= 75);  // 75%
  REQUIRE(nb_non_prime <= 25);  // 25%
}
