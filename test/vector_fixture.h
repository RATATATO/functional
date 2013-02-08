#ifndef FUNCTIONAL_VECTOR_FIXTURE_H_
#define FUNCTIONAL_VECTOR_FIXTURE_H_

#include <algorithm>
#include <vector>

namespace test {
using Vector= std::vector<int>;
class VectorFixture {
 public:
  test::Vector origin;
  test::Vector modified;

 public:
  VectorFixture() {
    constexpr uint64_t size = 16;

    origin.reserve(size);
    origin.resize(size);
    std::iota(origin.begin(), origin.end(), 1);

    modified = origin;
  } 
};

} // namespace test

#endif

