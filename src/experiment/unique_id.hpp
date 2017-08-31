# ifndef UNIQUE_ID_HPP
# define UNIQUE_ID_HPP

#include <stdexcept>
#include <exception>
#include <memory>
#include <limits>
#include <unordered_set>
#include <boost/format.hpp>


class id_already_exists 
: public virtual std::exception
{
  public:
    const char * what() const noexcept override {
      return "pass an already existing object to make_id causes <id_already_exists> to throw";
    }
};

template <class T>
class unique_id 
{
  public:
    typedef std::unordered_set<T> id_keeper_type;
    typedef typename id_keeper_type::size_type integer_type;
    typedef T        value_type;
    typedef T const& const_reference;
    typedef T&       reference;
    typedef T*       pointer;
    typedef const T* const_pointer;

    unique_id():m_content(nullptr), m_id(0) {}
    unique_id(pointer content, integer_type id):m_content(content),m_id(id) {}
    unique_id(unique_id const&) = default;
    ~unique_id() {}

    static bool exists(reference ref) {
      return s_id_keeper.find(ref) != s_id_keeper.end();
    }
    static unique_id make_id(reference ref) {
      if (exists(ref)) {
        throw id_already_exists();
      }
      auto result = s_id_keeper.insert(ref);
      return unique_id(new T(ref), current_id());
    }
    integer_type id() const {
      return m_id;
    }
    reference content() {
      return *m_content;
    }
    bool operator== (unique_id const& rhs)const {
      return id() == rhs.id();
    }
    bool operator!= (unique_id const& rhs)const {
      return !(*this == rhs);
    }
    friend std::ostream& operator<< (std::ostream& os, unique_id const& id) {
      return os << boost::format("unique_id(%1%)") % id.id();
    }

  private:
    static const integer_type s_max_id_value = 
      std::numeric_limits<integer_type>::max();
    static id_keeper_type s_id_keeper;
    static integer_type current_id() {
      return s_id_keeper.size();
    }

    pointer m_content;
    integer_type m_id;

};

namespace std {
template<class T>
  struct hash<unique_id<T>>
  :public hash<typename unique_id<T>::integer_type> {};
};

template<class T> typename unique_id<T>::id_keeper_type unique_id<T>::s_id_keeper;

# endif

