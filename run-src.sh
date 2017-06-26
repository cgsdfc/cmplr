#!/bin/sh

verb=$2
under=$1
logf=./errlog
defsrcf=./test/c-source

if [ $# -ne 1 ];then
  under=$defsrcf
fi

echo `date` > $logf

kases="$under/*.c $under/*.h"

for x in $kases;do
  printf "[case-$x]"
  ./bin/tokenizer $x > /dev/null 2>> $logf

  if [ $? -ne 0 ];then 
    errmsg="[FAIL] at $x" 
    echo $errmsg
    echo $errmsg >> $logf
  else
    echo [PASS]
  fi
done


