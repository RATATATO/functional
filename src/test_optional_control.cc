/*!
  \file test_optional_control.cc
  \brief Unit tests for 'functional/optional_control.h'
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestOptionalControl

#include <algorithm>
#include <functional>
#include <vector>
#include <boost/test/unit_test.hpp>
#include "test/vector_fixture.h"
#include "functional/optional_control.h"

using std::move;

namespace {
test::Vector increase(test::Vector&& src) {
  std::transform(
    src.begin(), src.end(), src.begin(), [](uint64_t i) { return i + 1; }
  );
  return test::Vector(move(src));
}

bool is_increased(const test::Vector& v) {
  uint64_t r = 2;
  return all_of(v.begin(), v.end(), [&r](uint64_t i) { return i == r++; });
}

bool check_sum(uint64_t s, const test::Vector& v)
  { return (v.size() * (v.size() + 1) / 2) == s; }

uint64_t sum(const test::Vector& v) {
  return std::accumulate(
    v.begin(), v.end(), 0, [](uint64_t s, uint64_t i) { return s + i; }
  );
}

test::Vector throw_exception(test::Vector&& src)
  { throw 10; return test::Vector(move(src)); }

} // namespace

BOOST_AUTO_TEST_CASE(verifyInjectOption) {
  using List = std::list<int>;
  List l;
  boost::optional<List> a = functional::inject_optional(
    [](List&& l) { l.push_back(10); return std::list<int>(move(l)); },
    std::move(l)
  );

  BOOST_CHECK(a);
  BOOST_CHECK(functional::hasSome(a));
  BOOST_CHECK(a.get().back() == 10);
}

BOOST_AUTO_TEST_CASE(verifyInjectOptionThrownException) {
  boost::optional<int> a = functional::inject_optional(
    [] { throw 15; return 21; }
  );

  BOOST_CHECK(!a);
  BOOST_CHECK(!functional::hasSome(a));
  BOOST_CHECK(functional::isNone(a));
}

BOOST_FIXTURE_TEST_SUITE(vector_suit, test::VectorFixture)

BOOST_AUTO_TEST_CASE(verifyFmapForRvalueReference) {
  boost::optional<test::Vector> src = boost::make_optional(move(origin));
  auto dst = functional::fmap(move(src), ::increase);

  BOOST_CHECK(dst);
  BOOST_CHECK(::is_increased(dst.get()));
}

BOOST_AUTO_TEST_CASE(verifyFmapForRvalueReferenceThrownException) {
  boost::optional<test::Vector> src = boost::make_optional(move(origin));
  auto dst = functional::fmap(move(src), ::throw_exception);

  BOOST_CHECK(!dst);
}

BOOST_AUTO_TEST_CASE(verifyFmapForLvalueReference) {
  boost::optional<test::Vector> src = boost::make_optional(move(origin));
  auto s = functional::fmap(src, ::sum);

  BOOST_CHECK(s);
  BOOST_REQUIRE(src.get().begin() != src.get().end());
  BOOST_CHECK(::check_sum(s.get(), src.get()));
}

BOOST_AUTO_TEST_CASE(verifyBind) {
  auto move_vector = [](test::Vector&& v) -> boost::optional<test::Vector> {
    try { test::Vector new_v(move(v)); return boost::make_optional(new_v); }
    catch(...) { return boost::none; }
  };
  boost::optional<test::Vector> src = boost::make_optional(move(origin));
  auto r = functional::bind(move(src), move_vector);

  BOOST_REQUIRE(r.get().begin() != r.get().end());
}

BOOST_AUTO_TEST_CASE(verifyBindForLvalueReference) {
  auto copy_vector = [](
    const test::Vector& v
  ) -> boost::optional<test::Vector> {
    try { test::Vector new_v(v); return boost::make_optional(new_v); }
    catch(...) { return boost::none; }
  };

  boost::optional<test::Vector> src = boost::make_optional(move(origin));
  auto r = functional::bind(src, copy_vector);

  BOOST_REQUIRE(src.get().begin() != src.get().end());
  BOOST_REQUIRE(r.get().begin() != r.get().end());
}

BOOST_AUTO_TEST_SUITE_END()

