/*!
  \file test_compose.cc
  \brief compose.hのユニットテスト。
 */
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestCompose

#include <algorithm>
#include <list>
#include <type_traits>
#include <functional>
#include <boost/test/unit_test.hpp>
#include "functional/compose.h"

using std::move;

namespace {
using List = std::list<int>;

auto push = [](::List l, int i) { l.push_back(i); return std::move(l); }; 
auto push1 = [](::List l) { l.push_back(1); return std::move(l); }; 
auto push1_ref = [](::List& l) -> ::List& { l.push_back(1); return l; }; 
auto back = [](const ::List&l ) { return l.back(); };

::List push1_fn(::List l) { l.push_back(1); return std::move(l); }
} // namespace

BOOST_AUTO_TEST_CASE(verifyId0) {
  auto id = functional::id<::List>();

  ::List l;
  auto result = id(move(l));
  BOOST_CHECK(result.size() == 0);
}

BOOST_AUTO_TEST_CASE(verifyId1) {
  auto id = functional::id<::List&>();

  ::List l;
  ::List& result = id(l);
  BOOST_CHECK(&result == &l);
  BOOST_CHECK(result.size() == 0);
}

BOOST_AUTO_TEST_CASE(verifyComposeOperator0) {
  auto push = ::push1 < functional::id<::List>();

  ::List l;
  auto result = push(move(l));
  BOOST_CHECK(result.size() == 1);
  BOOST_CHECK(result.back() == 1);
}

BOOST_AUTO_TEST_CASE(verifyComposeOperator1) {
  auto pushsh = ::push1 < (::push1 < functional::id<::List>());

  ::List l;
  auto result = pushsh(move(l));
  BOOST_CHECK(result.size() == 2);
  BOOST_CHECK(result.back() == 1);
}

BOOST_AUTO_TEST_CASE(verifyComposeOperator2) {
  auto pushsh = ::push1_ref < (::push1_ref < functional::id<::List&>());

  ::List l;
  auto& result = pushsh(l);
  BOOST_CHECK(&result == &l);
  BOOST_CHECK(result.size() == 2);
  BOOST_CHECK(result.back() == 1);
}

BOOST_AUTO_TEST_CASE(verifyComposeOperator3) {
  auto pushback = ::back < (::push1 < functional::id<::List>());

  ::List l;
  auto result = pushback(l);
  BOOST_CHECK(result == 1);
}

BOOST_AUTO_TEST_CASE(verifyComposeOperatorForFunction) {
  auto pushsh = ::push1_fn < (::push1_fn < functional::id<::List>());

  ::List l;
  auto result = pushsh(l);
  BOOST_CHECK(result.size() == 2);
  BOOST_CHECK(result.back() == 1);
}

BOOST_AUTO_TEST_CASE(verifyComposeOperatorForBindedFunction) {
  auto push1 = std::bind(::push, std::placeholders::_1, 3);
  auto pushsh = push1 < (push1 < functional::id<::List>());

  ::List l;
  auto result = pushsh(move(l));
  BOOST_CHECK(result.size() == 2);
  BOOST_CHECK(result.back() == 3);
}

