#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>  // for is_same
#include <iostream>

using std::cout;
using std::endl;
using boost::is_same;

namespace experiment {
namespace mpl {
namespace detail {
template <int Index, typename... Elements>
struct vector_element;

template <typename T0, typename... Rest>
struct vector_element<0, T0, Rest...> {
  typedef T0 type;
};

template <typename T0, typename T1, typename... Rest>
struct vector_element<1, T0, T1, Rest...> {
  typedef T1 type;
};

template <typename T0, typename T1, typename T2, typename... Rest>
struct vector_element<2, T0, T1, T2, Rest...> {
  typedef T2 type;
};

/////////////////////// second implement ///////////////////////

struct null_element {};

template <typename... Elements>
struct vector_impl;

template <typename Head, typename... Tail>
struct vector_impl<Head, Tail...> {
  typedef vector_impl<Tail...> tail;
  typedef Head head;
};

template <>
template vector_impl<> {
  typedef null_element type;
};

template <int Cur, int End, typename... Elements>
struct nth_element<Cur, End, vector_impl<Elements...>> {
  typedef typename nth_element < Cur + 1, End,
      vector_impl<Elements...>::tail::head type;
};

template <int Cur, int End, typename... Elements>
struct nth_element<End, End, vector_impl<Elements...>> {
  typedef typename vector_impl<Elements...>::head type;
};
}

template <typename... Elements>
struct vector {
  static const int size = sizeof...(Elements);

  template <typename Back>
  struct push_back {
    typedef vector<Elements..., Back> type;
  };

  template <int Index>
  struct at {
    typedef typename detail::vector_element<Index, Elements...>::type type;
  };

  struct front {
    typedef typename at<0>::type type;
  };

  struct back {
    typedef typename at<size - 1>::type type;
  };
};
}
}

int main(int argc, char **argv) {
  typedef experiment::mpl::vector<int, double> vec2;
  BOOST_STATIC_ASSERT(
      boost::is_same<typename vec2::template at<0>::type, int>::value);

  BOOST_STATIC_ASSERT(
      is_same<typename vec2::template at<1>::type, double>::value);

  BOOST_STATIC_ASSERT(is_same<typename vec2::front::type, int>::value);

  BOOST_STATIC_ASSERT(is_same<typename vec2::back::type, double>::value);

  BOOST_STATIC_ASSERT(vec2::size == 2);

  typedef typename vec2::template push_back<float>::type vec3;

  BOOST_STATIC_ASSERT(is_same<typename vec3::back::type, float>::value);
}
