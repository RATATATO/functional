/*!
  \file test_state.cc
  \brief Unit tests for controls of a state monad.
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestState

#include <algorithm>
#include <vector>
#include <boost/test/unit_test.hpp>
#include "test/vector_fixture.h"
#include "functional/state.h"

using std::vector;
using std::iota;
using functional::make_state;
using test::Vector;

namespace {
auto pop = [] {
  return make_state<int, Vector>(
    [](Vector t) {
      int i = t.back(); t.pop_back();
      return std::pair<int, Vector>(i, std::move(t));
    }
  );
};

auto popplus = [](int x) {
  return make_state<int, Vector>(
    [x](Vector t) {
      int i = t.back(); t.pop_back();
      return std::pair<int, Vector>(i + x, std::move(t));
    }
  );
};

template <typename F>
functional::State< Vector, int> popop(int x, F f) {
  return make_state<int, Vector>(
    [f, x](Vector t) {
      int i = t.back(); t.pop_back();
      return std::pair<int, Vector>(f(x, i), std::move(t));
    }
  );
}

auto push = [](int x) {
  return make_state<int, Vector>(
    [x](Vector t)
      { t.push_back(x); return std::pair<int, Vector>(x, std::move(t)); }
  );
};

} // namespace


BOOST_FIXTURE_TEST_SUITE(vector_suit, test::VectorFixture)

BOOST_AUTO_TEST_CASE(verifyMakeState) {
  auto state = make_state<int, Vector>(
    [](Vector t) { return std::pair<int, Vector>(t.back(), std::move(t)); }
  );

  auto result = state(modified);
  BOOST_CHECK(result.first == static_cast<int>(modified.size()));
  BOOST_REQUIRE(origin.size() == modified.size());
  BOOST_CHECK(std::equal(origin.begin(), origin.end(), modified.begin()));
}

BOOST_AUTO_TEST_CASE(verifyBind) {
  auto poppush = functional::bind(functional::bind(::pop(), ::popplus), ::push);
  auto result = poppush(modified);
  BOOST_CHECK(result.first == *(origin.end() - 1) + *(origin.end() - 2));
  BOOST_CHECK(result.second.size() == origin.size() - 1);
}

BOOST_AUTO_TEST_CASE(verifyBindPassedVariadicArgument) {
  auto poppluspush = functional::bind(
    functional::bind(
      ::pop(), ::popop<decltype(std::plus<int>())>, std::plus<int>()
    ),
    ::push
  );
  auto result = poppluspush(modified);
  BOOST_CHECK(result.first == *(origin.end() - 1) + *(origin.end() - 2));
  BOOST_CHECK(result.second.size() == origin.size() - 1);

  auto popminuspush = functional::bind(
    functional::bind(
      ::pop(), ::popop<decltype(std::minus<int>())>, std::minus<int>()
    ),
    ::push
  );
  auto result1 = popminuspush(modified);
  BOOST_CHECK(result1.first == *(origin.end() - 1) - *(origin.end() - 2));
  BOOST_CHECK(result1.second.size() == origin.size() - 1);
}

BOOST_AUTO_TEST_CASE(verifyBindForVoidFunction) {
  auto poppp= functional::bind_void(
    functional::bind_void(::pop(), ::pop), ::pop
  );
  auto result = poppp(modified);
  BOOST_CHECK(result.first == *(origin.end() - 3));
  BOOST_CHECK(result.second.size() == origin.size() - 3);
}

BOOST_AUTO_TEST_CASE(verifyBindForVoidFunctionPassedVariadicArgument) {
  auto poppp= functional::bind_void(
    functional::bind_void(::pop(), ::pop), ::push, 100
  );
  auto result = poppp(modified);
  BOOST_CHECK(result.first == 100);
  BOOST_CHECK(result.second.back() == 100);
  BOOST_CHECK(result.second.size() == origin.size() - 1);
}

BOOST_AUTO_TEST_CASE(verifyFmap) {
  auto popplusone = functional::fmap(::pop(), [](int x) { return x + 1; });
  auto result = popplusone(modified);

  BOOST_CHECK(result.first == origin.back() + 1);
  BOOST_CHECK(result.second.size() == origin.size() - 1);
}

BOOST_AUTO_TEST_CASE(verifyFmap2) {
  auto popplustwo = functional::fmap(
    functional::fmap(::pop(), [](int x) { return x + 1; }),
    [](int x) { return x + 1; }
  );
  auto result = popplustwo(modified);

  BOOST_CHECK(result.first == origin.back() + 2);
  BOOST_CHECK(result.second.size() == origin.size() - 1);
}

BOOST_AUTO_TEST_SUITE_END()

