#ifndef EXPERIMENT_UNIQUE_MAP_HPP
#define EXPERIMENT_UNIQUE_MAP_HPP 1
#include <functional>  // for hash, less, equal_to
#include <map>
#include <unordered_map>
#include <vector>
namespace experiment {
template <class T>
struct unique_traits {
  typedef std::size_t unique_id;
  typedef std::map<T, unique_id> map_type;
};  // default Traits for unique_id

template <class T, class Traits = unique_traits<T> >
class unique_map {
 public:
  typedef typename Traits::unique_id unique_id;
  typedef T value_type;
  typedef value_type& reference;
  typedef value_type const& const_reference;
  typedef std::size_t size_type;

 public:
  unique_map();
  // content should be const
  const_reference operator[](unique_id id) const;
  reference operator[] (unique_id id) const;
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

 private:
  typedef std::vector<T> vector;
  typedef typename Traits::map_type map;

 public:
  static constexpr const unique_id npos = unique_id(-1);
  typedef typename vector::const_iterator value_iterator;
  typedef typename map::const_iterator item_iterator;

  value_iterator vbegin() const;
  value_iterator vend() const;
  item_iterator ibegin() const;
  item_iterator iend() const;
  void swap(unique_map& other);

 private:
  vector m_vector;
  map m_map;
  unique_id m_count;
};
template <class T>
inline void swap(unique_map<T> lhs, unique_map<T> rhs);
}  // namespace experiment
#include "impl/unique_map.ipp"
#endif  // EXPERIMENT_UNIQUE_MAP_HPP
