#ifndef EXPERIMENT_IMPL_UNIQUE_MAP_IPP
#define EXPERIMENT_IMPL_UNIQUE_MAP_IPP 1

namespace experiment {
template <UNIQUE_MAP_TEMPLATE_PARAM>
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::unique_map()
    : m_count(0), m_vector(), m_map() {}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::reference
    unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::operator[](unique_id id) {
  return m_vector[id];
}

template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::const_reference
    unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::operator[](unique_id id) const {
  return m_vector[id];
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::unique_id
    unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::operator[](const_reference ref) {
  auto result = m_map.emplace(ref, m_count);
  if (result.second) {  // emplace succeeded
    m_vector.push_back(ref);
    return m_count++;
  }
  return result.first->second;
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::unique_id
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::find(const_reference ref) const {
  auto result = m_map.find(ref);
  return result == m_map.end() ? npos : result->second /* unique_id */;
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::size_type
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::size() const {
  return m_count;
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
void unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::reserve(size_type size) {
  m_vector.reserve(size);
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::reference
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::at(unique_id id) {
  return m_vector.at(id);
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::const_reference
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::at(unique_id id) const {
  return m_vector.at(id);
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::mutable_value_iterator
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::vbegin() {
  return m_vector.begin();
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::mutable_value_iterator
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::vend() {
  return m_vector.end();
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::map_type&
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::items() {
  return m_map;
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::vector_type&
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::values() {
  return m_vector;
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::value_iterator
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::vbegin() const {
  return m_vector.begin();
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::value_iterator
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::vend() const {
  return m_vector.end();
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::mutable_item_iterator
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::ibegin(){
  return m_map.begin();
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::mutable_item_iterator
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::iend() {
  return m_map.end();
}

template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::item_iterator
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::ibegin() const {
  return m_map.begin();
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
typename unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::item_iterator
unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::iend() const {
  return m_map.end();
}
template <UNIQUE_MAP_TEMPLATE_PARAM>
void unique_map<UNIQUE_MAP_TEMPLATE_ARGS>::swap(unique_map& other) {
  std::swap(m_vector, other.m_vector);
  std::swap(m_map, other.m_map);
}
template <class T>
inline void swap(unique_map<T> lhs, unique_map<T> rhs) {
  lhs.swap(rhs);
}
}  // namespace experiment
#endif  // EXPERIMENT_IMPL_UNIQUE_MAP_IPP
