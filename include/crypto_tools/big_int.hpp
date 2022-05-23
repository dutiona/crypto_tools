#pragma once

#include <boost/multiprecision/cpp_int.hpp>

#include "crypto_tools/crypto_tools_export.hpp"

namespace crypto_tools
{
CRYPTO_TOOLS_EXPORT using big_int = boost::multiprecision::cpp_int;
CRYPTO_TOOLS_EXPORT using checked_big_int =
    boost::multiprecision::checked_cpp_int;
}  // namespace crypto_tools
