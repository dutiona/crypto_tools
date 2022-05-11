#include <string>

#include "crypto_tools/crypto_tools.hpp"

#include <catch2/catch_all.hpp>
#include <fmt/core.h>

TEST_CASE("Compute prime number", "[is prime]")
{
  auto bit_length = 16;
  auto prime = generate_large_prime(bit_length);
  if (prime) {
    fmt::print("Prime number: {}\n", *prime);
    REQUIRE(is_prime(*prime));
  } else {
    fmt::print("Error generating prime number", *prime);
  }
}
