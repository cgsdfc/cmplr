# Compiler Library
## Overview
As there are already so many state-of-art compilers in the world such as [clang](https://clang.llvm.org/),
[gcc](https://gcc.gnu.org/), [Intel compiler](https://software.intel.com/en-us/c-compilers), MSVC and so on, what 
on earth is the need to write such a compiler library? The reason is that I need to write a compiler in 
the school-assignment
sense in C and I need some supports. As is known to all, the C programming language does not provide builtin data structure 
so much as C++ does. The discourage of our course to use any third party library even generators like `yacc` or `lex`
intensifies the need to a self-contained library, which should aid compiler writing (again in school-assignment sense). 
Without generators or libraries, writing compiler for a very tiny language in C becomes a non trivial task. What's more, manually crafted parsing subroutines are tedious and seem to recover from errors badly.
Here is where this library comes to aid.

## Features
The `cmplr` project comes along with a supporting library call `utillib`, which features basic data structure like `vector`, `string`, `slist` and generators for several classes of grammar. Here is a list of features you may want to check out.

- Includes basic data structures that closely follow the convensions of [C++ STL](en.cppreference.com/w/cpp/language). 
- Includes a tesing framework that behaves like [GTest](https://github.com/google/googletest).
- Includes a way to define BNF rules and grammatical symbols in source code and generate parsers from them.
- Has a working [LL(1)](https://en.wikipedia.org/wiki/LL_parse) generator to build parser table.
- Supports serializing C data structures to [JSON](https://www.json.org/index.html) string.
For more details about APIs, please refer to WIKI, which is... under development.

## Install
This project uses both cmake and autoconf as build system.
- cmake `mkdir build; cmake ..; make install`.
- autoconf `autoreconf -ivf; ./configure; make install`.

If you use autoconf way, please make sure you have autotool tool chains installed on your system.

Currently `make check` is supported only with autoconf, so there is no check if you use cmake way.

## Development
The topics of current development includes:
- Finish the JSON parser and integrate it with the JSON dumper.
- Make an ll1_testing module to aid testing of ll1_parser.
- Write a hash map that is more space friendly.
- Implement the LALR generator.
- More enhancement of testings and documents.

## Application
There is already an application of `utillib`. It is a toy language called `PL-0` which is an over-simplication of
the Pascal programming language. Currently needed implementation are some semantic actions during parsing and the 
interpreter to execute the so called `PCODE`, which is again a set of over-simplified intermediate instruction.

## About
The name of this project is the abbreviation of compiler but it can also be pronunced as different abbreviations,
such as 'Cannot Make Programs Link Recursively', which is a major problem of my life when I did not know how to
link programs with complicated inter-library dependencies and make them work.

