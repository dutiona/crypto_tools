#include <catch2/catch_all.hpp>
#include <fmt/core.h>

#include "crypto_tools/crypto_tools.hpp"

#include <algorithm>
#include <string>

TEST_CASE("Compute shamir shares", "[shares]")
{
  using std::begin;

  auto bit_length = 1024u;
  auto secret = generate_large_prime(bit_length);
  if (!secret) {
    fmt::print("Could not generate the secret!\n");
    if (secret.error() == generate_prime_error::failure_after_too_many_tries) {
      fmt::print("failure_after_too_many_tries!\n");
    }
  } else {
    fmt::print("SECRET <{}>\n\n\n", secret->str());

    auto shares = compute_shamir_shares(bit_length, 3, 6, bit_length);
    if (shares) {
      fmt::print("Prime <{}>\n", shares->prime.str());
      for (auto e : shares->shares) {
        fmt::print("Share <{}>\n", e.str());
      }

      std::random_device rd;
      std::mt19937 gen(rd());

      auto fragments = shamir_shares_t {
          .threshold = shares->threshold, .prime = shares->prime, .shares = {}};
      std::copy_n(begin(shares->shares),
                  fragments.threshold,
                  std::back_inserter(fragments.shares));

      auto recovered_secret = recover_secret_from_fragments(fragments);
      if (recovered_secret) {
        fmt::print("Recovered secret: <{}>\n", recovered_secret->str());
      }
    } else {
      fmt::print("Error!\n");
    }
  }
}
