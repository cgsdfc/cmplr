#ifndef EXPERIMENT_IMPL_SYMBOL_IPP
#define EXPERIMENT_IMPL_SYMBOL_IPP 1
namespace experiment {
inline std::ostream &operator<<(std::ostream &os, symbol const &s) {
  return os << s.m_str;
}
template <class Archive>
void symbol::serialize(Archive &ar, const unsigned version) {
  ar &m_prop;
  ar &m_str;
}

inline void symbol::set(symbol_property prop) { m_prop = prop; }
inline bool symbol::unknown() const {
  return m_prop == symbol_property::unknown;
}
inline bool symbol::terminal() const {
  return m_prop == symbol_property::terminal ||
         m_prop == symbol_property::reserved || m_prop == symbol_property::eof;
}
inline bool symbol::nonterminal() const {
  return m_prop == symbol_property::nonterminal ||
         m_prop == symbol_property::optional ||
         m_prop == symbol_property::start;
}
inline bool symbol::start() const { return m_prop == symbol_property::start; }
inline bool symbol::eof() const { return m_prop == symbol_property::eof; }
}  // namespace experiment
#endif  // EXPERIMENT_IMPL_SYMBOL_IPP
