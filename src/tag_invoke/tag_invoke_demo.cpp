#include <utility>
#include <iostream>

#include "src/tag_invoke.h"

template <class T>
constexpr std::string_view type_name() {
  using namespace std;
  string_view p = __PRETTY_FUNCTION__;
  return string_view(p.data() + 49, p.find(';', 49) - 49);
}

namespace {

inline struct test_cpo_t {
  // An optional default implementation
  template <typename T>
  friend bool tag_invoke(test_cpo_t, const T& x) noexcept {
    return false;
  }

  template <typename... T>
  auto operator()(T&&... x) const
      noexcept(unifex::is_nothrow_tag_invocable_v<test_cpo_t, T&&...>)
          -> unifex::tag_invoke_result_t<test_cpo_t, T&&...> {
    // Dispatch to the call to tag_invoke() passing the CPO as the
    // first parameter.
    return tag_invoke(test_cpo_t{}, std::forward<T>(x)...);
  }
} test_cpo;

// inline constexpr struct test_cpo_t {
//   template <typename CPO = test_cpo_t, typename... Targs>
//   decltype(auto) operator()(Targs&&... args) const
//       noexcept(noexcept(tag_invoke(CPO{}, std::forward<Targs>(args)...)))
//   /* -> decltype(auto
//                // tag_invoke(CPO{}, std::forward<Targs>(args)...)
//    )*/
//   {
//     return tag_invoke(CPO{}, std::forward<Targs>(args)...);
//   }
// } test_cpo;

struct X {
  friend void tag_invoke(unifex::tag_t<test_cpo>, X) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
  friend bool& tag_invoke(unifex::tag_t<test_cpo>, X, int a) noexcept {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    static bool b;
    b = a > 0;
    return b;
  }
};

struct Y {};
}  // anonymous namespace

int main() {
  X x;
  test_cpo(x);
  std::cout << type_name<decltype(test_cpo(x, 12))>() << std::endl;
  test_cpo(x, 12);

  Y y;

  test_cpo(y);

  return 0;
}