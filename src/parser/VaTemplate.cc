#include <iostream>
namespace parser {

template <typename... E> struct count {};

template <typename T, typename... R> struct count<T, R...> {
  static const int value = 1 + count<R...>::value;
};

template <> struct count<> { static const int value = 0; };
template <typename... E> struct tuple { const int len = count<E...>::value; };

void print() { std::cout << std::endl; }

template <typename T, typename... Args>
void print(const T &value, const Args &... args) {
  std::cout << value << ' ';
  print(args...);
}
}

int main() {
  parser::tuple<int, int, int, int, int, int, int> i;
  std::cout << "length of i is " << i.len << std::endl;

  parser::print("one", "two", "three");
  parser::print("one");
  parser::print();
}
