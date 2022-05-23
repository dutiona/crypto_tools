#include <boost/random.hpp>
#include <catch2/catch_all.hpp>
#include <fmt/core.h>

#include "crypto_tools/crypto_tools.hpp"

#include <algorithm>
#include <random>
#include <string>

using crypto_tools::big_int;
using crypto_tools::expected_secret;

template<class Int>
static constexpr big_int fast_pow2(Int&& n)
{
  big_int one = 1;
  return (one << std::forward<Int>(n));
}

big_int generate_random_secret(unsigned bit_length)
{
  auto gen = std::mt19937_64 {std::random_device {}()};

  // translate bit length into an interval
  auto dist = boost::random::uniform_int_distribution<big_int> {
      fast_pow2(bit_length - 1), fast_pow2(bit_length)};

  return dist(gen);
}

TEST_CASE("Compute shamir shares", "[shares]")
{
  using std::begin;

  auto bit_length = 1024u;
  auto secret = generate_random_secret(bit_length);
  REQUIRE(boost::multiprecision::msb(secret) + 1 == bit_length);

  fmt::print("SECRET (bit_length<{}>) <{}>\n\n\n",
             boost::multiprecision::msb(secret) + 1,
             secret.str());

  auto shares = crypto_tools::compute_shamir_shares(secret, 3, 6, bit_length);
  if (shares) {
    fmt::print("Prime <{}>\n", shares->prime.str());
    for (auto e : shares->shares) {
      fmt::print("Share <{}>\n", e.str());
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    auto fragments = crypto_tools::shamir_shares_t {
        .threshold = shares->threshold, .prime = shares->prime, .shares = {}};
    std::copy_n(begin(shares->shares),
                fragments.threshold,
                std::back_inserter(fragments.shares));

    auto recovered_secret =
        crypto_tools::recover_secret_from_fragments(fragments);
    if (recovered_secret) {
      fmt::print("Recovered secret: <{}>\n", recovered_secret->str());

      REQUIRE(secret == *recovered_secret);
    }
  } else {
    fmt::print("Error!\n");
  }
}
