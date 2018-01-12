#include "cling-core.h"

int main(int argc, char argv[])
{
  struct frontend frontend;
  struct data_flow data_flow;

  frontend_init(&frontend, fopen(argv[1], "r"));
  frontend_parse(&frontend);
  backend




}
