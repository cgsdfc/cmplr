#!/bin/sh

if [ $# -ne 1 ];then
  echo "Usage run-src.sh dir"
  echo "test the tokenizer with all the C source and headers under a dir"
  exit 0
fi

under=$1

test -d $under || echo "$1 no such dir" && exit 1

kases="$under/*.c $under/*.h"

for x in $kases;do
  printf "[case-$x]"
  ./bin/tokenizer $x > /dev/null &2>1
  if [ $? -ne 0 ];then 
    echo [FAIL]
  else
    echo [PASS]
  fi
done


