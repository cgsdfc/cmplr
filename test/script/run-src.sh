#!/bin/sh

logf=./errlog
under=./c-source/gxemul-src
binfile=./bin/tknzr/tokenizer

echo `date` > $logf

kases="$under/*.c $under/*.h"

for x in $kases;do
  printf "[case-$x]"
  $binfile $x 2>> $logf >/dev/null
  if [ $? -ne 0 ];then 
    errmsg="[FAIL] at $x" 
    echo $errmsg
    echo $errmsg >> $logf
  else
    echo [PASS]
  fi
done

