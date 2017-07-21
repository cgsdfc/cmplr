#include "Language.h"

int main(int argc, char **argv)
{
  Language lang;
  lang.define(Symbol("Goal")) |= Rule() << Symbol("Sum");

}

