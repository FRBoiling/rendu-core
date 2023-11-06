/*
* Created by boil on 2023/11/2.
*/

#ifndef RENDU_OPERATORS_H
#define RENDU_OPERATORS_H

#include "common/define.h"

RD_NAMESPACE_BEGIN

  namespace operators_detail {

    template<typename T>
    class empty_base {
    };

    struct true_t {
    };
    struct false_t {
    };
  } // namespace operators_detail

  template<class T>
  struct is_chained_base {
    typedef operators_detail::false_t value;
  };


  template<class T, class U, class B = operators_detail::empty_base<T> >
  struct equality_comparable2 : B {
    friend constexpr bool operator==(const U &y, const T &x) { return x == y; }

    friend constexpr bool operator!=(const U &y, const T &x) { return !static_cast<bool>(x == y); }

    friend constexpr bool operator!=(const T &y, const U &x) { return !static_cast<bool>(y == x); }
  };

  template<class T, class B = operators_detail::empty_base<T> >
  struct equality_comparable1 : B {
    friend constexpr bool operator!=(const T &x, const T &y) { return !static_cast<bool>(x == y); }
  };

  template<class T, class U, class B = operators_detail::empty_base<T> >
  struct less_than_comparable2 : B {
    friend constexpr bool operator<=(const T &x, const U &y) { return !static_cast<bool>(x > y); }

    friend constexpr bool operator>=(const T &x, const U &y) { return !static_cast<bool>(x < y); }

    friend constexpr bool operator>(const U &x, const T &y) { return y < x; }

    friend constexpr bool operator<(const U &x, const T &y) { return y > x; }

    friend constexpr bool operator<=(const U &x, const T &y) { return !static_cast<bool>(y < x); }

    friend constexpr bool operator>=(const U &x, const T &y) { return !static_cast<bool>(y > x); }
  };

  template<class T, class B = operators_detail::empty_base<T> >
  struct less_than_comparable1 : B {
    friend constexpr bool operator>(const T &x, const T &y) { return y < x; }

    friend constexpr bool operator<=(const T &x, const T &y) { return !static_cast<bool>(y < x); }

    friend constexpr bool operator>=(const T &x, const T &y) { return !static_cast<bool>(x < y); }
  };


// Provide a specialization of 'is_chained_base<>'
// for a 4-type-argument operator template.
# define BOOST_OPERATOR_TEMPLATE4(template_name4)           \
  template<class T, class U, class V, class W, class B>     \
  struct is_chained_base< template_name4<T, U, V, W, B> > { \
    typedef operators_detail::true_t value;                 \
  };

// Provide a specialization of 'is_chained_base<>'
// for a 3-type-argument operator template.
# define BOOST_OPERATOR_TEMPLATE3(template_name3)        \
  template<class T, class U, class V, class B>           \
  struct is_chained_base< template_name3<T, U, V, B> > { \
    typedef operators_detail::true_t value;              \
  };

// Provide a specialization of 'is_chained_base<>'
// for a 2-type-argument operator template.
# define BOOST_OPERATOR_TEMPLATE2(template_name2)     \
  template<class T, class U, class B>                 \
  struct is_chained_base< template_name2<T, U, B> > { \
    typedef operators_detail::true_t value;           \
  };

// Provide a specialization of 'is_chained_base<>'
// for a 1-type-argument operator template.
# define BOOST_OPERATOR_TEMPLATE1(template_name1)  \
  template<class T, class B>                       \
  struct is_chained_base< template_name1<T, B> > { \
    typedef operators_detail::true_t value;        \
  };

// BOOST_OPERATOR_TEMPLATE(template_name) defines template_name<> such that it
// can be used for specifying both 1-argument and 2-argument forms. Requires the
// existence of two previously defined class templates named '<template_name>1'
// and '<template_name>2' which must implement the corresponding 1- and 2-
// argument forms.
//
// The template type parameter O == is_chained_base<U>::value is used to
// distinguish whether the 2nd argument to <template_name> is being used for
// base class chaining from another boost operator template or is describing a
// 2nd operand type. O == true_t only when U is actually an another operator
// template from the library. Partial specialization is used to select an
// implementation in terms of either '<template_name>1' or '<template_name>2'.
//
# define BOOST_OPERATOR_TEMPLATE(template_name)                                       \
template <class T                                                                     \
         ,class U = T                                                                 \
         ,class B = operators_detail::empty_base<T>                                   \
         ,class O = typename is_chained_base<U>::value                                \
         >                                                                            \
struct template_name;                                                                 \
                                                                                      \
template<class T, class U, class B>                                                   \
struct template_name<T, U, B, operators_detail::false_t>                              \
  : template_name##2<T, U, B> {};                                                     \
                                                                                      \
template<class T, class U>                                                            \
struct template_name<T, U, operators_detail::empty_base<T>, operators_detail::true_t> \
  : template_name##1<T, U> {};                                                        \
                                                                                      \
template <class T, class B>                                                           \
struct template_name<T, T, B, operators_detail::false_t>                              \
  : template_name##1<T, B> {};                                                        \
                                                                                      \
template<class T, class U, class B, class O>                                          \
struct is_chained_base< template_name<T, U, B, O> > {                                 \
  typedef operators_detail::true_t value;                                             \
};                                                                                    \
BOOST_OPERATOR_TEMPLATE2(template_name##2)                                            \
BOOST_OPERATOR_TEMPLATE1(template_name##1)


  BOOST_OPERATOR_TEMPLATE(less_than_comparable)
  BOOST_OPERATOR_TEMPLATE(equality_comparable)

#undef BOOST_OPERATOR_TEMPLATE
#undef BOOST_OPERATOR_TEMPLATE4
#undef BOOST_OPERATOR_TEMPLATE3
#undef BOOST_OPERATOR_TEMPLATE2
#undef BOOST_OPERATOR_TEMPLATE1


RD_NAMESPACE_END
#endif //RENDU_OPERATORS_H
