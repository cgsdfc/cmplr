#include <utillib/graph.h>
#include <utillib/test.h>

UTILLIB_TEST(edge_list_json)
{
  struct utillib_edge_list edge_list;
  utillib_edge_list_init(&edge_list);
  enum { A, B, C, D, N };
  utillib_edge_list_add_edge(&edge_list, A, B, "edge1");
  utillib_edge_list_add_edge(&edge_list, B, B, "edge2");
  utillib_edge_list_add_edge(&edge_list, B, C, "edge3");
  utillib_edge_list_add_edge(&edge_list, A, B, "edge4");
  utillib_edge_list_add_edge(&edge_list, C, D, "edge5");
  utillib_edge_list_add_edge(&edge_list, B, A, "edge6");
  
  struct utillib_json_value *val=utillib_edge_list_json_object_create(
      &edge_list, NULL, utillib_json_string_create_adaptor);
  utillib_json_pretty_print(val);

}

UTILLIB_TEST_DEFINE(Utillib_Graph) {
  UTILLIB_TEST_BEGIN(Utillib_Graph)
  UTILLIB_TEST_RUN(edge_list_json)
  UTILLIB_TEST_END(Utillib_Graph)
  UTILLIB_TEST_RETURN(Utillib_Graph)
}
