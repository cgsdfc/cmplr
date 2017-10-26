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
#ifndef COLOR_H_
#define COLOR_H_

#define GREEN "32"
#define RED "31"
#define PURPLE "35"
#define CYAN "36"
#define WHITE "37"

/** \macro COLOR_FORMAT_BOLD
 * Decorates a "%s" format string with COLOR
 */
#define COLOR_FORMAT_BOLD(COLOR) "%s \e[1;" COLOR "m%s\e[0m"
/** \macro COLOR_STRING_BOLD
 * Decorates a string literal with COLOR
 */
#define COLOR_STRING_BOLD(c, s) "\e[1;" c "m" s "\e[0m"

/**
 * \macro
 * The unbold version of the above macros
 */
#define COLOR_FORMAT_UNBOLD(COLOR) "%s \e[0;" COLOR "m%s\e[0m"
#define COLOR_STRING_UNBOLD(c, s) "\e[0;" c "m" s "\e[0m"

#endif
