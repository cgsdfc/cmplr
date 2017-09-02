#ifndef EXPERIMENT_INDENT_HPP
#define EXPERIMENT_INDENT_HPP 1
namespace experiment {

template <int Space>
struct indent {
  unsigned& m_level;
  std::ostream& m_os;
  std::string m_space;

  indent(std::ostream& os, unsigned& level)
      : m_level(++level), m_os(os), m_space(Space, ' ') {
    for (int i = m_level; i--; os << m_space)
      ;
  }
  ~indent() {
    --m_level;
    m_os << '\n';
  }
};

typedef indent<1> indent1;
typedef indent<2> indent2;
typedef indent<4> indent4;
}  // namespace experiment
#endif  // EXPERIMENT_INDENT_HPP
