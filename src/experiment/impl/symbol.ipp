#ifndef EXPERIMENT_IMPL_SYMBOL_IPP
#define EXPERIMENT_IMPL_SYMBOL_IPP 1
namespace experiment {
inline symbol_property symbol::property() const { return m_prop; }
inline std::ostream &operator<<(std::ostream &os, symbol const &s) {
  return os << "\"" << s.m_str << "\"";
}
inline bool operator<(symbol const &left, symbol const &right) {
  return left.m_str < right.m_str;
}
inline bool operator==(symbol const &left, symbol const &right) {
  return left.m_str == right.m_str;
}
template <class Archive>
void symbol::serialize(Archive &ar, const unsigned version) {
  ar &m_prop;
  ar &m_str;
}
inline bool symbol::optional() const {
  return m_prop == symbol_property::optional;
}
inline void symbol::set(symbol_property prop) const { m_prop = prop; }
inline bool symbol::unknown() const {
  return m_prop == symbol_property::unknown;
}
inline bool symbol::terminal() const {
  return m_prop == symbol_property::terminal;
}
inline bool symbol::toplevel() const {
  return m_prop == symbol_property::toplevel;
}
inline bool symbol::nonterminal() const {
  return m_prop == symbol_property::nonterminal ||
         m_prop == symbol_property::optional ||
         m_prop == symbol_property::toplevel;
}
}  // namespace experiment
namespace std {
template <>
struct hash<experiment::symbol> {
  std::hash<std::string> m_hash;
  std::size_t operator()(experiment::symbol const &s) const {
    return m_hash(s.m_str);
  }
};
}  // namespace std
#endif  // EXPERIMENT_IMPL_SYMBOL_IPP
