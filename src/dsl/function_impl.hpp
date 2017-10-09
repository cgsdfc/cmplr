namespace dsl {
class node_base {
public:
  using name_type = const char *;
  template <class T> using share_handle_type = std::shared_ptr<T>;
};

// every step add more things to the root node,
// and the object holding the root node outlives
// its children. and finally we get a tree.

class argument_list;
class function_body;
class function_name : public node_base {
private:
  name_type m_name;
  share_handle_type<argument_list> m_args;

public:
  function_name(name_type name) : m_name(name) {}
  template <class... Args> argument_list operator(Args &&... args);
  template <class... Args>
  argument_list operator(name_type arg, Args &&... args) {
    m_args = std::make_shared<argument_list>(
        0); // means the argument_list is not empty
    m_args->parse(arg, std::forward<Args>(args)...);
    return *m_args;
  }
};

class argument_list : public node_base {
private:
  share_handle_type<function_body> m_body;
  share_handle_type<std::vector<share_handle_type<node_base>>> m_list;

public:
  bool empty() const { return bool(m_list); }
  explicit argument_list(int) : m_list(new decltype(m_list)) {}
  template <class... Args> void parse(Args &&... args);
  template <class... Args>
  function_body operator(name_type name, Args &&... args) {
    m_body = std::make_shared<function_body>(0);
    m_body->parse(name, std::forward<Args>(args)...);
    return *m_body;
  }
};

class function_body {
public:
  template <class... Args> void parse();
    template<class... Args>
      void parse(
    template<class... Args>
      call_expression operator(Args&&... args);
};

function_definition function_impl(const char *name) {
  return function_definition(name);
}
