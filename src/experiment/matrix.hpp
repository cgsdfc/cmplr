#ifndef EXPERIMENT_MATRIX_HPP
#define EXPERIMENT_MATRIX_HPP 1
#include <vector>
// maybe replace it with boost::graph::matrix
template <class T>
struct matrix : public std::vector<std::vector<T>> {
  typedef std::vector<std::vector<T>> base_t;
  typedef typename base_t::value_type row_type;
  typedef typename base_t::size_type size_type;
  matrix(size_type nrow, size_type ncol) : base_t(nrow, row_type(ncol)) {}
};


#endif // EXPERIMENT_MATRIX_HPP
