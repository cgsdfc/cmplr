#!/bin/sh

verb=$2
under=$1
logf=./errlog
defsrcf=./c-source/gxemul-src

if [ $# -ne 1 ];then
  under=$defsrcf
fi

echo `date` > $logf

kases="$under/*.c $under/*.h"

for x in $kases;do
  printf "[case-$x]"
  ./bin/tokenizer/tokenizer $x 2>> $logf >/dev/null

  if [ $? -ne 0 ];then 
    errmsg="[FAIL] at $x" 
    echo $errmsg
    echo $errmsg >> $logf
  else
    echo [PASS]
  fi
done


