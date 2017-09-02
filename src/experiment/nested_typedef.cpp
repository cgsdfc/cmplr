#include <boost/serialization/strong_typedef.hpp>
#include <boost/type_traits/is_same.hpp>

template <class T>
class enclosing_class {
 public:
  typedef int nested_type;
};

BOOST_STRONG_TYPEDEF(int, new_int);

void f(enclosing_class<int>::nested_type) {}
void f(new_int) {}

int main(int ac, char** av) {
  static_assert(boost::is_same<enclosing_class<int>::nested_type,
                               enclosing_class<double>::nested_type>::value,
                "putting a typedef in template class \
      generate new types");
  f(new_int());
}
