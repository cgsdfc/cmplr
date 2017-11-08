/*
   Cmplr Library
   Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA

*/
#ifndef UTILLIB_ARGP_H
#define UTILLIB_ARGP_H

/**
 * \file utillib/argp.h
 * Light but convenient wrapper on gnu-c's
 * argp library.
 */
#include <argp.h>
typedef struct argp utillib_argp;
typedef struct argp_option utillib_argp_option;
typedef argp_parser_t utillib_argp_parser_t;
typedef struct argp_child utillib_argp_child;
typedef char *(*utillib_help_filter)(int key, const char *text, void *input);
typedef error_t utillib_argp_error_t;
typedef struct argp_state utillib_argp_state;
#define UTILLIB_ARGP_STATIC_ARGP static_argp
#define UTILLIB_ARGP_STATIC_OPTIONS static_argp_options
#define UTILLIB_ARGP_STATIC_PARSER static_argp_parser
#define UTILLIB_ARGP_PROGRAM_VERSION(VERSION)                                  \
  (argp_program_version = (VERSION));
#define UTILLIB_ARGP_PROGRAM_BUG_ADDRESS(ADDRESS)                              \
  (argp_program_bug_address = (ADDRESS));

#define UTILLIB_ARGP_OPTION_BEGIN()                                            \
  static utillib_argp_error_t UTILLIB_ARGP_STATIC_PARSER(                      \
      int, char *, utillib_argp_state *);                                      \
  static const utillib_argp_option UTILLIB_ARGP_STATIC_OPTIONS[] = {

#define UTILLIB_ARGP_OPTION_ELEM(NAME, KEY, ARG, DOC)                          \
  {(NAME), (KEY), (ARG), (0), (DOC), (0)},

#define UTILLIB_ARGP_OPTION_END()                                              \
  UTILLIB_ARGP_OPTION_ELEM(0, 0, 0, 0)                                         \
  }                                                                            \
  ;

#define UTILLIB_ARGP_OPTION_REGISTER(ARG_DOC, DOC)                             \
  static const utillib_argp UTILLIB_ARGP_STATIC_ARGP = {                       \
      .parser = UTILLIB_ARGP_STATIC_PARSER,                                    \
      .options = (UTILLIB_ARGP_STATIC_OPTIONS),                                \
      .args_doc = (ARG_DOC),                                                   \
      .doc = (DOC)};

#define UTILLIB_ARGP_PARSE(ARGC, ARGV, INPUT)                                  \
  argp_parse(&UTILLIB_ARGP_STATIC_ARGP, ARGC, ARGV, 0, 0, INPUT)

/* access to argp_state structure */
#define UTILLIB_ARGP_STATE_INPUT(S) ((S)->input)

#endif /* UTILLIB_ARGP_H */
