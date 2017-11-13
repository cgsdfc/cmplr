# 
#  Cmplr Library
#  Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
#  02110-1301 USA

project(cmplr C)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file (STRINGS "VERSION" CMPLR_VERSION)
include_directories(src)

if (MSVC OR WIN32)
  set(CMPLR_PLATFORM "WINDOWS")
else(MSVC OR WIN32)
  set(CMPLR_PLATFORM "LINUX")
endif(MSVC OR WIN32)

if (CMPLR_PLATFORM STREQUAL "LINUX")
  set(CMAKE_C_FLAGS "-g3 -std=c99")
endif(CMPLR_PLATFORM STREQUAL "LINUX")

add_subdirectory(src/utillib)
add_subdirectory(src/pascal)

