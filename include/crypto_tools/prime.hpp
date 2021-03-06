#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <tl/expected.hpp>

#include "crypto_tools/big_int.hpp"
#include "crypto_tools/crypto_tools_export.hpp"

namespace crypto_tools
{

CRYPTO_TOOLS_EXPORT bool is_prime(const big_int& n);

enum class CRYPTO_TOOLS_EXPORT generate_prime_error
{
  bit_length_too_small,
  failure_after_too_many_tries
};

CRYPTO_TOOLS_EXPORT using expected_unsigned_long_long =
    tl::expected<big_int, generate_prime_error>;

// k is the desired bit length
CRYPTO_TOOLS_EXPORT expected_unsigned_long_long
generate_large_prime(unsigned bit_length = 64, bool safe = false);

}  // namespace crypto_tools
