#ifndef EXPERIMENT_UNIQUE_MAP_HPP
#define EXPERIMENT_UNIQUE_MAP_HPP 1
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_same.hpp>
#include <map>
#include <memory>  // for shared_ptr
#include <vector>

#define UNIQUE_MAP_TEMPLATE_PARAM \
  class T, class Integer, class Map, class Vector
#define UNIQUE_MAP_TEMPLATE_ARGS T, Integer, Map, Vector
namespace experiment {
template <class T, class Integer = std::size_t,
          class Map = std::map<T, Integer>, class Vector = std::vector<T>>
class unique_map {
 public:
  typedef Map map_type;
  typedef Vector vector_type;
  typedef Integer unique_id;
  typedef typename Vector::size_type size_type;
  typedef typename Vector::value_type value_type;
  typedef typename Vector::reference reference;
  typedef typename Vector::const_reference const_reference;
  BOOST_STATIC_ASSERT(boost::is_same<T, value_type>::value);
  BOOST_STATIC_ASSERT(boost::is_integral<Integer>::value);
  BOOST_STATIC_ASSERT(boost::is_same<T, typename map_type::key_type>::value);

 public:
  unique_map();
  // content should be const
  const_reference operator[](unique_id id) const;
  reference operator[](unique_id id);
  // note: the call to this mutable overload
  // should not affect the identity of the returned
  // object. the result of calling hash, equal_to, less
  // on the object before and after returned should make
  // no difference;
  unique_id operator[](const_reference ref);
  unique_id find(const_reference ref) const;
  size_type size() const;
  void reserve(size_type size);
  const_reference at(unique_id id) const;
  reference at(unique_id id);

 public:
  static constexpr const unique_id npos = unique_id(-1);
  typedef typename vector_type::const_iterator value_iterator;
  typedef typename vector_type::iterator mutable_value_iterator;
  typedef typename map_type::const_iterator item_iterator;
  typedef typename map_type::iterator mutable_item_iterator;

  mutable_value_iterator vbegin();
  mutable_value_iterator vend();
  value_iterator vbegin() const;
  value_iterator vend() const;
  item_iterator ibegin() const;
  item_iterator iend() const;
  mutable_item_iterator ibegin();
  mutable_item_iterator iend();
  map_type& items();
  vector_type& values();
  void swap(unique_map& other);

 private:
  vector_type m_vector;
  map_type m_map;
  unique_id m_count;
};
template <class T>
inline void swap(unique_map<T> lhs, unique_map<T> rhs);
}  // namespace experiment
#include "impl/unique_map.ipp"
#endif  // EXPERIMENT_UNIQUE_MAP_HPP
