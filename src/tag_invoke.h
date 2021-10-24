#pragma once

#include <type_traits>

#define UNIFEX_DECLVAL(...) static_cast<__VA_ARGS__ (*)() noexcept>(nullptr)()

namespace unifex {

// We don't care about volatile, and not handling volatile is
// less work for the compiler.
template <class T>
struct remove_cvref {
  using type = T;
};
template <class T>
struct remove_cvref<T const> {
  using type = T;
};
// template <class T> struct remove_cvref<T volatile> { using type = T; };
// template <class T> struct remove_cvref<T const volatile> { using type = T; };
template <class T>
struct remove_cvref<T &> {
  using type = T;
};
template <class T>
struct remove_cvref<T const &> {
  using type = T;
};
// template <class T> struct remove_cvref<T volatile&> { using type = T; };
// template <class T> struct remove_cvref<T const volatile&> { using type = T;
// };
template <class T>
struct remove_cvref<T &&> {
  using type = T;
};
template <class T>
struct remove_cvref<T const &&> {
  using type = T;
};
// template <class T> struct remove_cvref<T volatile&&> { using type = T; };
// template <class T> struct remove_cvref<T const volatile&&> { using type = T;
// };

template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <bool B>
struct _if {
  template <typename, typename T>
  using apply = T;
};
template <>
struct _if<true> {
  template <typename T, typename>
  using apply = T;
};

template <bool B, typename T, typename U>
using conditional_t = typename _if<B>::template apply<T, U>;

namespace _tag_invoke {
void tag_invoke();

struct _fn {
  template <typename CPO, typename... Args>
  constexpr auto operator()(CPO cpo, Args &&...args) const
      noexcept(noexcept(tag_invoke((CPO &&) cpo, (Args &&) args...)))
          -> decltype(tag_invoke((CPO &&) cpo, (Args &&) args...)) {
    return tag_invoke((CPO &&) cpo, (Args &&) args...);
  }
};

template <typename CPO, typename... Args>
using tag_invoke_result_t =
    decltype(tag_invoke(UNIFEX_DECLVAL(CPO &&), UNIFEX_DECLVAL(Args &&)...));

using yes_type = char;
using no_type = char (&)[2];

template <typename CPO, typename... Args>
auto try_tag_invoke(int)  //
    noexcept(noexcept(tag_invoke(UNIFEX_DECLVAL(CPO &&),
                                 UNIFEX_DECLVAL(Args &&)...)))
        -> decltype(static_cast<void>(tag_invoke(UNIFEX_DECLVAL(CPO &&),
                                                 UNIFEX_DECLVAL(Args &&)...)),
                    yes_type{});

template <typename CPO, typename... Args>
no_type try_tag_invoke(...) noexcept(false);

template <template <typename...> class T, typename... Args>
struct defer {
  using type = T<Args...>;
};

struct empty {};
}  // namespace _tag_invoke

namespace _tag_invoke_cpo {
inline constexpr _tag_invoke::_fn tag_invoke{};
}
using namespace _tag_invoke_cpo;

template <auto &CPO>
using tag_t = remove_cvref_t<decltype(CPO)>;

// Manually implement the traits here rather than defining them in terms of
// the corresponding std::invoke_result/is_invocable/is_nothrow_invocable
// traits to improve compile-times. We don't need all of the generality of the
// std:: traits and the tag_invoke traits are used heavily through libunifex
// so optimising them for compile time makes a big difference.

using _tag_invoke::tag_invoke_result_t;

template <typename CPO, typename... Args>
inline constexpr bool is_tag_invocable_v =
    (sizeof(_tag_invoke::try_tag_invoke<CPO, Args...>(0)) ==
     sizeof(_tag_invoke::yes_type));

template <typename CPO, typename... Args>
struct tag_invoke_result
    : conditional_t<is_tag_invocable_v<CPO, Args...>,
                    _tag_invoke::defer<tag_invoke_result_t, CPO, Args...>,
                    _tag_invoke::empty> {};

template <typename CPO, typename... Args>
using is_tag_invocable = std::bool_constant<is_tag_invocable_v<CPO, Args...>>;

template <typename CPO, typename... Args>
inline constexpr bool is_nothrow_tag_invocable_v =
    noexcept(_tag_invoke::try_tag_invoke<CPO, Args...>(0));

template <typename CPO, typename... Args>
using is_nothrow_tag_invocable =
    std::bool_constant<is_nothrow_tag_invocable_v<CPO, Args...>>;

template <template <typename...> class T>
struct meta_quote {
  template <typename... As>
  struct bind_front {
    template <typename... Bs>
    using apply = T<As..., Bs...>;
  };
  template <typename... As>
  using apply = T<As...>;
};

template <template <typename> class T>
struct meta_quote1 {
  template <typename...>
  struct bind_front;
  template <typename A0>
  struct bind_front<A0> {
    template <int = 0>
    using apply = T<A0>;
  };
  template <typename A0>
  using apply = T<A0>;
};

template <template <typename, typename> class T>
struct meta_quote2 {
  template <typename...>
  struct bind_front;
  template <typename A0>
  struct bind_front<A0> {
    template <typename A1>
    using apply = T<A0, A1>;
  };
  template <typename A0, typename A1>
  struct bind_front<A0, A1> {
    template <int = 0>
    using apply = T<A0, A1>;
  };
  template <typename A0, typename A1>
  using apply = T<A0, A1>;
};

template <template <typename, typename, typename> class T>
struct meta_quote3 {
  template <typename...>
  struct bind_front;
  template <typename A0>
  struct bind_front<A0> {
    template <typename A1, typename A2>
    using apply = T<A0, A1, A2>;
  };
  template <typename A0, typename A1>
  struct bind_front<A0, A1> {
    template <typename A2>
    using apply = T<A0, A1, A2>;
  };
  template <typename A0, typename A1, typename A2>
  struct bind_front<A0, A1, A2> {
    template <int = 0>
    using apply = T<A0, A1, A2>;
  };
  template <typename A0, typename A1, typename A2>
  using apply = T<A0, A1, A2>;
};

template <template <typename, typename...> class T>
struct meta_quote1_ {
  template <typename A, typename... Bs>
  using apply = T<A, Bs...>;
  template <typename...>
  struct bind_front;
  template <typename A, typename... Bs>
  struct bind_front<A, Bs...> {
    template <typename... Cs>
    using apply = T<A, Bs..., Cs...>;
  };
};

// template <typename CPO, typename... Args>
// UNIFEX_CONCEPT tag_invocable =
//     (sizeof(_tag_invoke::try_tag_invoke<CPO, Args...>(0)) ==
//      sizeof(_tag_invoke::yes_type));

template <typename Fn>
using meta_tag_invoke_result =
    meta_quote1_<tag_invoke_result_t>::bind_front<Fn>;
}  // namespace unifex
