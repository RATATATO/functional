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
using boost::none;
using boost::optional;
using boost::make_optional;
using test::Vector;

BOOST_AUTO_TEST_CASE(verifyHasSome0) {
  auto opt0 = make_optional(1);
  BOOST_CHECK(functional::hasSome(opt0));
  auto opt1 = make_optional(Vector());
  BOOST_CHECK(functional::hasSome(opt1));
}

BOOST_AUTO_TEST_CASE(verifyHasSome1) {
  auto opt0 = make_optional(1);
  auto opt1 = make_optional(Vector());
  BOOST_CHECK(functional::hasSome(opt0, opt1));
}

BOOST_AUTO_TEST_CASE(verifyIsNone) {
  optional<int> opt0 = none;
  BOOST_CHECK(functional::isNone(opt0));
  optional<Vector> opt1 = none;
  BOOST_CHECK(functional::isNone(opt1));
}

BOOST_AUTO_TEST_CASE(verifyInjectOption0) {
  using List = std::list<int>;
  List l;
  auto opt = functional::inject_optional(
    [](List&& l) { l.push_back(10); return List(move(l)); },
    std::move(l)
  );

  BOOST_CHECK(opt);
  BOOST_CHECK(opt.get().back() == 10);
}

BOOST_AUTO_TEST_CASE(verifyInjectOption1) {
  using List = std::list<int>;
  List l;
  auto opt = functional::inject_optional(
    [](List&& l, uint64_t i) { l.push_back(i); return List(move(l)); },
    std::move(l), 10
  );

  BOOST_CHECK(opt);
  BOOST_CHECK(opt.get().back() == 10);
}

BOOST_AUTO_TEST_CASE(verifyInjectOptionThrownException) {
  auto opt = functional::inject_optional([] { throw 15; return 21; });

  BOOST_CHECK(!opt);
}

BOOST_FIXTURE_TEST_SUITE(vector_suit, test::VectorFixture)

BOOST_AUTO_TEST_CASE(verifyFmapForRvalueReference) {
  auto increase = [](Vector&& src) {
    std::transform(
      src.begin(), src.end(), src.begin(), [](uint64_t i) { return i + 1; }
    );
    return Vector(move(src));
  };
  optional<Vector> src = make_optional(move(modified));
  auto dst = functional::fmap(move(src), increase);

  BOOST_CHECK(dst);
  BOOST_REQUIRE(dst.get().size() == origin.size());
  BOOST_CHECK(
    std::equal(
      dst.get().begin(), dst.get().end(), origin.begin(),
      [](uint64_t i, uint64_t j) { return i == j + 1; }
    )
  );
}

BOOST_AUTO_TEST_CASE(verifyFmapForRvalueReferenceThrownException) {
  auto throw_exception = [](Vector&& src)
    { throw 10; return Vector(move(src)); };
  optional<Vector> src = make_optional(move(modified));
  auto dst = functional::fmap(move(src), throw_exception);

  BOOST_CHECK(!dst);
}

BOOST_AUTO_TEST_CASE(verifyFmapForConstLvalueReference) {
  auto sum = [](const Vector& v)
    { return std::accumulate(v.begin(), v.end(), 0); };
  optional<Vector> src = make_optional(move(modified));
  auto s = functional::fmap(src, sum);

  BOOST_CHECK(src);
  BOOST_CHECK(s);
  BOOST_CHECK(sum(origin) == s.get());
}

BOOST_AUTO_TEST_CASE(verifyBind) {
  auto move_vector = [](Vector&& v) -> optional<Vector> {
    try { Vector new_v(move(v)); return make_optional(new_v); }
    catch(...) { return none; }
  };
  optional<Vector> src = make_optional(move(modified));
  auto copied = functional::bind(move(src), move_vector);

  BOOST_REQUIRE(copied);
  BOOST_REQUIRE(copied.get().size() == origin.size());
  BOOST_CHECK(
    std::equal(copied.get().begin(), copied.get().end(), origin.begin())
  );
}

BOOST_AUTO_TEST_CASE(verifyBindForConstLvalueReference) {
  auto copy_vector = [](const Vector& v) -> optional<Vector> {
    try { Vector new_v(v); return make_optional(new_v); }
    catch(...) { return none; }
  };

  optional<Vector> src = make_optional(move(modified));
  auto copied = functional::bind(src, copy_vector);

  BOOST_REQUIRE(src);
  BOOST_REQUIRE(copied);
  BOOST_REQUIRE(copied.get().size() == origin.size());
  BOOST_CHECK(
    std::equal(copied.get().begin(), copied.get().end(), origin.begin())
  );
}

BOOST_AUTO_TEST_SUITE_END()

