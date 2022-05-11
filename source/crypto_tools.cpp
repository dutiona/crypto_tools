#include <string>

#include "crypto_tools/crypto_tools.hpp"

exported_class::exported_class()
    : m_name("crypto_tools")
{
}

auto exported_class::name() const -> const char*
{
  return m_name.c_str();
}
