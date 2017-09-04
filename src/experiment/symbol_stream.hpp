#ifndef EXPERIMENT_SYMBOL_STREAM_HPP
#define EXPERIMENT_SYMBOL_STREAM_HPP 1
#include <iostream>
#include <iterator>
#include <sstream>
#include <boost/iterator/iterator_facade.hpp>

#include "language.hpp"
namespace experiment {
class symbol_stream :
  public boost::iterator_facade<symbol_stream,
  const language::symbol_unique_id,
  boost::forward_traversal_tag>
  /* std::iterator_traits<std::istream_iterator<std::string>>:: */
  /* iterator_category> */ 
  {
 public:
  typedef language const& const_language;
  typedef language::symbol_unique_id symbol_unique_id;
  typedef std::istream_iterator<std::string> istream_iterator;
  typedef std::istream istream_type;

 private:
  friend class boost::iterator_core_access;
  bool stream_exhausted() const {
    return m_iter == istream_iterator();
  }
  void increment() {
   if (stream_exhausted())
     return;
   m_iter++;
  }
  symbol_unique_id const& dereference() const{
    if (stream_exhausted()) {
      m_id = m_language.eof();
    } else {
      m_id =  m_language.find_symbol(*m_iter);
    } return m_id;
  }
  bool equal(symbol_stream const& other) const{
    return m_id == other.m_id;
  }
  istream_iterator m_iter;
  const_language m_language;
  mutable symbol_unique_id m_id;

 public:
  symbol_stream(const_language language)
      : m_iter(), m_language(language) {}  // end of stream
  symbol_stream(istream_type& is, const_language language)
      : m_iter(is), m_language(language) {}
};
}  // namespace experiment
#endif  // EXPERIMENT_SYMBOL_STREAM_HPP
