/*!
  \file compose.cc
  \brief 合成関数のパフォーマンス検証。
 */
#include <algorithm>
#include <functional>
#include <list>
#include <boost/timer/timer.hpp>
#include "functional/compose.h"

namespace {
constexpr uint64_t size = 200000;
using List = std::list<uint64_t>;

auto init = [](List l) { 
  for(int i = 0; i < size; ++i) { l.push_back(i); } return std::move(l); 
};
auto transform = [](::List l) {
  std::transform(
    l.begin(), l.end(), l.begin(), [](uint64_t i) { return i + 1; }
  );
  return std::move(l);
};
auto sum = [](List l) { return std::accumulate(l.begin(), l.end(), 1); };
auto print = [](uint64_t i) { std::cout << i << std::endl; };

void func(::List l) { print(sum(transform(init(std::move(l))))); }
auto composed0
  = ::print < (::sum < (::transform < (::init < functional::id<::List>())));
auto composed1
  = functional::id<::List>() > ::init > ::transform > ::sum > ::print;
} // namespace

int main(void) {
  {
    boost::timer::auto_cpu_timer t("just function: %w [s]");
    ::List l;
    func(std::move(l));
  }
  std::cout << std::endl;
  {
    boost::timer::auto_cpu_timer t("my composition operator <: %w [s]");
    ::List l;
    composed0(std::move(l));
  }
  std::cout << std::endl;
  {
    boost::timer::auto_cpu_timer t("my composition operator >: %w [s]");
    ::List l;
    composed1(std::move(l));
  }
  std::cout << std::endl;

  return 0;
}

