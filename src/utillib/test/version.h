#ifndef UTILLIB_VERSION_H
#define UTILLIB_VERSION_H

#define UTILLIB_VERSION_CAT(MAJOR, MINOR, MICRO)                               \
  (((MAJOR)*1000000) + ((MINOR)*10000) + ((MICRO)*100))
