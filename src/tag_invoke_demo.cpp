#include "src/tag_invoke.h"

namespace {
inline constexpr struct test_cpo {
  template <typename... ARGS>
  auto operator()(ARGS&&... args) {}
} test;

struct X {
  friend void tag_invoke(test_cpo, X) {}
  friend constexpr bool tag_invoke(test_cpo, X, int a) noexcept {
    return a > 0;
  }
};

struct Y {};
}  // anonymous namespace

int main() {
  X x;
  text_cpo(X);
  text_cpo(X, 12);
  return 0;
}