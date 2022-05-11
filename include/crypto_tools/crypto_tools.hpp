#pragma once

#include <string>

#include <tl/expected.hpp>

#include "crypto_tools/crypto_tools_export.hpp"

CRYPTO_TOOLS_EXPORT bool is_prime(unsigned long long n);

enum class CRYPTO_TOOLS_EXPORT generate_error
{
  failure_after_too_many_tries
};

CRYPTO_TOOLS_EXPORT using expected_unsigned_long_long =
    tl::expected<unsigned long long, generate_error>;

// k is the desired bit length
CRYPTO_TOOLS_EXPORT expected_unsigned_long_long
generate_large_prime(unsigned long long k);
