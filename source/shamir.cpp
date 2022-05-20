#include "crypto_tools/shamir.hpp"

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>
#include <fmt/core.h>

#include "crypto_tools/prime.hpp"

#include <algorithm>
#include <random>
#include <vector>

static boost::multiprecision::cpp_int eval_poly_at(
    const std::vector<boost::multiprecision::cpp_int>& poly,
    int x,
    const boost::multiprecision::cpp_int& prime)
{
  boost::multiprecision::cpp_int acc = 0;
  {
    auto b = rbegin(poly);
    auto e = rend(poly);
    for (; b != e; ++b) {
      auto coeff = *b;
      acc *= x;
      acc += coeff;
      acc %= prime;
    }
  }
  return acc;
}

expected_shamir_shares compute_shamir_shares(
    const boost::multiprecision::cpp_int& secret,
    unsigned threshold,
    unsigned number,
    unsigned bit_length)
{
  auto prime = generate_large_prime(bit_length);
  if (!prime)
    return tl::unexpected {shamir_error::could_not_generate_prime};

  if (threshold > number)
    return tl::unexpected {shamir_error::share_number_smaller_than_threshold};

  std::mt19937_64 gen {std::random_device {}()};
  auto dist =
      boost::random::uniform_int_distribution<boost::multiprecision::cpp_int>(
          0, *prime - 1);
  auto poly = std::vector<boost::multiprecision::cpp_int>(threshold);
  poly[0] = secret;

  for (int i = 1; i < threshold; ++i) {
    poly[i] = dist(gen);
  }
  auto shares = std::vector<boost::multiprecision::cpp_int>(number);

  for (int i = 0; i < number; ++i) {
    shares[i] = eval_poly_at(poly, i + 1, *prime);
  }

  return shamir_shares_t {threshold, *prime, shares};
}

static std::pair<boost::multiprecision::cpp_int, boost::multiprecision::cpp_int>
extended_gcd(boost::multiprecision::cpp_int a, boost::multiprecision::cpp_int b)
{
  boost::multiprecision::cpp_int x = 0, last_x = 1, y = 1, last_y = 1;
  while (b != 0) {
    boost::multiprecision::cpp_int quot = a / b;
    a = b;
    b = a % b;
    x = last_x - quot * x;
    last_x = x;
    y = last_y - quot * y;
    last_y = y;
  }
  return {last_x, last_y};
}

static boost::multiprecision::cpp_int divmod(
    const boost::multiprecision::cpp_int& n,
    const boost::multiprecision::cpp_int& d,
    const boost::multiprecision::cpp_int& p)
{
  auto [inv, _] = extended_gcd(d, p);
  return n * inv;
}

static boost::multiprecision::cpp_int lagrange_interpolate(
    unsigned x,
    const std::vector<unsigned>& x_s,
    const std::vector<boost::multiprecision::cpp_int>& y_s,
    const boost::multiprecision::cpp_int& p)
{
  using std::begin;
  using std::end;

  auto k = x_s.size();

  auto PI =
      []<class T>(const std::vector<T>& vals) -> boost::multiprecision::cpp_int
  {
    boost::multiprecision::cpp_int acc = 1;
    for (auto v : vals) {
      acc *= v;
    }
    return acc;
  };

  auto nums = std::vector<boost::multiprecision::cpp_int> {};
  auto dens = std::vector<boost::multiprecision::cpp_int> {};
  for (int i = 0; i < k; ++i) {
    auto cur = x_s[i];
    auto others = std::vector<int> {};
    std::copy_if(begin(x_s),
                 end(x_s),
                 std::back_inserter(others),
                 [cur](auto val) { return val != cur; });
    auto tmp = std::vector<int> {};
    std::transform(begin(others),
                   end(others),
                   std::back_inserter(tmp),
                   [x](auto v) { return x - v; });
    nums.push_back(PI(tmp));

    tmp.clear();

    std::transform(begin(others),
                   end(others),
                   std::back_inserter(tmp),
                   [cur](int v) { return cur - v; });
    dens.push_back(PI(tmp));
  }
  auto den = PI(dens);
  boost::multiprecision::cpp_int num = 0;
  for (int i = 0; i < k; ++i) {
    num += divmod(nums[i] * den * y_s[i] % p, dens[i], p);
  }
  return (divmod(num, den, p) + p) % p;
}

expected_secret recover_secret_from_fragments(const shamir_shares_t& shares)
{
  if (shares.threshold > shares.shares.size()) {
    return tl::unexpected {shamir_error::not_enough_shares_to_recover_secret};
  }

  auto x_s = std::vector<unsigned>(shares.shares.size());
  std::generate(begin(x_s), end(x_s), [n = 1]() mutable { return n++; });
  return lagrange_interpolate(0, x_s, shares.shares, shares.prime);
}
