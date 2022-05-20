#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <tl/expected.hpp>

#include "crypto_tools/crypto_tools_export.hpp"

enum class CRYPTO_TOOLS_EXPORT shamir_error
{
  share_number_smaller_than_threshold,
  could_not_generate_prime,
  not_enough_shares_to_recover_secret
};

struct CRYPTO_TOOLS_EXPORT shamir_shares_t
{
  unsigned threshold;
  boost::multiprecision::cpp_int prime;
  std::vector<boost::multiprecision::cpp_int> shares;
};

CRYPTO_TOOLS_EXPORT using expected_shamir_shares =
    tl::expected<shamir_shares_t, shamir_error>;

CRYPTO_TOOLS_EXPORT expected_shamir_shares
compute_shamir_shares(const boost::multiprecision::cpp_int& secret,
                      unsigned threshold,
                      unsigned number,
                      unsigned bit_length = 1024);

CRYPTO_TOOLS_EXPORT using expected_secret =
    tl::expected<boost::multiprecision::cpp_int, shamir_error>;

CRYPTO_TOOLS_EXPORT expected_secret
recover_secret_from_fragments(const shamir_shares_t& shares);
