#!/bin/sh

if [ $# -ne 2 ];then
  echo "Usage run-src.sh dir verbose?"
  echo "test the tokenizer with all the C source and headers under a dir"
  exit 0
fi

verb=$2
under=$1
logf=./errlog
echo `date` > $logf

kases="$under/*.c $under/*.h"

for x in $kases;do
  printf "[case-$x]"
  if [ $verb -eq 1 ];then
    ./bin/tokenizer $x
  else
    ./bin/tokenizer $x > /dev/null 2>> $logf
  fi

  if [ $? -ne 0 ];then 
    errmsg="[FAIL] at $x" 
    echo $errmsg
    echo $errmsg >> $logf
  else
    echo [PASS]
  fi
done


