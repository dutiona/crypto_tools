#include <string>

#include "crypto_tools/crypto_tools.hpp"

auto main() -> int
{
  exported_class e;

  return std::string("crypto_tools") == e.name() ? 0 : 1;
}
