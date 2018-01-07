#include "cling-core.h"

int main(int argc, char argv[])
{
  struct cling_frontend frontend;
  struct cling_data_flow data_flow;

  cling_frontend_init(&frontend, fopen(argv[1], "r"));
  cling_frontend_parse(&frontend);
  cling_backend




}
