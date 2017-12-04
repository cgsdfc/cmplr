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

#include "strhash.h"
#include <stdlib.h>
#include <string.h>

size_t simple_strhash(char const *str) {
  size_t len=strlen(str);
  size_t hash=str[0] + str[len-1] +(len << 4);
  return hash;
}

size_t mysql_strhash(char const *key)
{ 
  size_t length=strlen(key);
  register size_t nr=1, nr2=4; 
  while (length--) 
  { 
    nr^= (((nr & 63)+nr2)*((size_t) (unsigned char) *key++))+ (nr << 8); 
    nr2+=3; 
  } 
  return nr; 
}

unsigned long php_strhash(char const *arKey)
{ 
  unsigned int nKeyLength=strlen(arKey);
  unsigned long h = 0, g; 
  char *arEnd=arKey+nKeyLength;
  while (arKey < arEnd) { 
    h = (h << 4) + *arKey++; 
    if ((g = (h & 0xF0000000))) { 
      h = h ^ (g >> 24); 
      h = h ^ g; 
    } 
  } 
  return h; 
} 

unsigned long lh_strhash(char const *str) 
{ 
  int i,l; 
  unsigned long ret=0; 
  unsigned short *s;


  if (str == NULL) return(0); 
  l=(strlen(str)+1)/2; 
  s=(unsigned short *)str; 
  for (i=0; i < l; ++i) 
    ret^=(s[i]<<(i&0x0f)); 
  return(ret); 
}
