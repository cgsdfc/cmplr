#ifndef EXPERIMENT_SUBJECT_TRAITS_HPP
#define EXPERIMENT_SUBJECT_TRAITS_HPP 1
#include "unique_map.hpp"
namespace experiment {
template <class Subject>
struct subject_traits {
  typedef Subject type;
  typedef unique_map<type> map_type;
  typedef typename map_type::unique_id unique_id;
  typedef typename map_type::value_iterator value_iterator;
  typedef typename map_type::item_iterator item_iterator;
};
}  // namespace experiment
#endif  // EXPERIMENT_SUBJECT_TRAITS_HPP
