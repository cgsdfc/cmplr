#ifndef EXPERIMENT_UNIQUE_MAP_HPP
#define EXPERIMENT_UNIQUE_MAP_HPP 1
#include <functional> // for hash, less, equal_to
#include <vector>
#include <unordered_map>
#include <map>

namespace experiment {
  // consider boost::bimap?
  // less powerful
  template<class T>
    struct unique_traits {
      typedef std::size_t unique_id;
      typedef std::map<T, unique_id> 
        map_type;
    }; // default Traits for unique_id

  template<class T, class Traits=unique_traits<T> >
    class unique_map {
      public:
        typedef typename Traits::unique_id unique_id;
        typedef  T value_type;
        typedef value_type& reference;
        typedef value_type const& const_reference;
        typedef std::size_t size_type;

      public:
        unique_map(): m_count(0), m_vector(), m_map() {}
        // content should be const
        const_reference operator[] (unique_id id) const {
          return m_vector[id]; // may throws
        }
        unique_id operator[] (const_reference ref) {
          auto result = m_map.emplace(ref, m_count);
          if (result.second) { // emplace succeeded
            m_vector.push_back(ref);
            return m_count++;
          }
          return result.first->second; 
        }
        size_type size() const {
          return m_count;
        }
        void reserve(size_type size) {
          m_vector.reserve(size);
        }

      private:
        typedef std::vector<T> vector;
        typedef typename Traits::map_type map;

      public:
        static constexpr const unique_id npos = unique_id(-1);
        typedef typename vector::const_iterator value_iterator;
        typedef typename map::const_iterator item_iterator;

        value_iterator vbegin() const {
          return m_vector.begin();
        }
        value_iterator vend() const {
          return m_vector.end();
        }
        item_iterator ibegin() const {
          return m_map.begin();
        }
        item_iterator iend() const {
          return m_map.end();
        }
        void swap(unique_map& other) {
          std::swap(m_vector, other.m_vector);
          std::swap(m_map, other.m_map);
        }

      private:
        vector m_vector;
        map m_map;
        unique_id m_count;

    };
  template<class T>
  inline void swap(unique_map<T> lhs, unique_map<T> rhs) {
    lhs.swap(rhs);
  }
} // namespace experiment
#endif // EXPERIMENT_UNIQUE_MAP_HPP 
