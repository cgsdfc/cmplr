#ifndef UTILLIB_UTILLIB_HPP
#define UTILLIB_UTILLIB_HPP
extern "C" {
#include <utillib/json.h>
#include <utillib/json_foreach.h>
}
#include <cstddef>

namespace utillib {
  class json_value {
    private:
      struct utillib_json_value * val_;
    public:
      explicit json_value(struct utillib_json_value * val);
      json_value();
      ~json_value();
      void destroy();
      void array_push_back(json_value element);
      void object_push_back(char const *key, json_value value);
      bool is_null() const;
      json_value operator[](char const *key);
      json_value operator[](std::size_t index);
  };
  template<class T>
    json_value json_value_create(T val);
      json_value json_array_create();
      json_value json_object_create();

  template<>
    json_value json_value_create<int> (int val)
    {
      return json_value(utillib_json_int_create(&val));
    }

  template<>
    json_value json_value_create<long> (long val)
    {
      return json_value(utillib_json_long_create(&val));
    }

  template<>
    json_value json_value_create<std::size_t> (std::size_t val)
    {
      return json_value(utillib_json_size_t_create(&val));
    }

  template<>
    json_value json_value_create<const char *> (const char * val)
    {
      return json_value(utillib_json_string_create(&val));
    }

  template<>
    json_value json_value_create<float> (float val)
    {
      return json_value(utillib_json_float_create(&val));
    }

  template<>
    json_value json_value_create<double> (double val)
    {
      return json_value(utillib_json_real_create(&val));
    }

} // utillib

#endif // UTILLIB_UTILLIB_HPP
