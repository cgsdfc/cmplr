#ifndef EXPERIMENT_IMPL_UNIQUE_MAP_IPP
#define EXPERIMENT_IMPL_UNIQUE_MAP_IPP 1

namespace experiment {
template <class T, class Traits>
unique_map<T, Traits>::unique_map() : m_count(0), m_vector(), m_map() {}
template <class T, class Traits> typename
unique_map<T, Traits>::reference unique_map<T, Traits>::operator[](
    unique_id id) {
  return m_vector[id];
}

template <class T, class Traits> typename
unique_map<T, Traits>::const_reference unique_map<T, Traits>::operator[](
    unique_id id) const {
  return m_vector[id];
}
template <class T, class Traits> typename
unique_map<T, Traits>::unique_id unique_map<T, Traits>::operator[](
    const_reference ref) {
  auto result = m_map.emplace(ref, m_count);
  if (result.second) {  // emplace succeeded
    m_vector.push_back(ref);
    return m_count++;
  }
  return result.first->second;
}
template <class T, class Traits> typename
unique_map<T, Traits>::unique_id unique_map<T, Traits>::find(
    const_reference ref) const {
  auto result = m_map.find(ref);
  return result == m_map.end() ? npos : result->second /* unique_id */;
}
template <class T, class Traits> typename
unique_map<T, Traits>::size_type unique_map<T, Traits>::size() const {
  return m_count;
}
template <class T, class Traits>
void unique_map<T, Traits>::reserve(size_type size) {
  m_vector.reserve(size);
}
template <class T, class Traits> typename
unique_map<T, Traits>::reference unique_map<T, Traits>::at(
    unique_id id)  {
  return m_vector.at(id);
}
template <class T, class Traits> typename
unique_map<T, Traits>::const_reference unique_map<T, Traits>::at(
    unique_id id) const {
  return m_vector.at(id);
}
template <class T, class Traits> typename
unique_map<T, Traits>::value_iterator unique_map<T, Traits>::vbegin() const {
  return m_vector.begin();
}
template <class T, class Traits> typename
unique_map<T, Traits>::value_iterator unique_map<T, Traits>::vend() const {
  return m_vector.end();
}
template <class T, class Traits> typename
unique_map<T, Traits>::item_iterator unique_map<T, Traits>::ibegin() const {
  return m_map.begin();
}
template <class T, class Traits> typename
unique_map<T, Traits>::item_iterator unique_map<T, Traits>::iend() const {
  return m_map.end();
}
template <class T, class Traits>
void unique_map<T, Traits>::swap(unique_map& other) {
  std::swap(m_vector, other.m_vector);
  std::swap(m_map, other.m_map);
}
template <class T>
inline void swap(unique_map<T> lhs, unique_map<T> rhs) {
  lhs.swap(rhs);
}
}  // namespace experiment
#endif  // EXPERIMENT_IMPL_UNIQUE_MAP_IPP
