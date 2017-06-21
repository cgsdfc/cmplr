#!/bin/sh

testfile="char_literal comment id keywords number operators "

testdir="test"
testbin="./bin/tokenizer"
nerr=0

for x in $testfile;do
  $testbin $testdir/$x.txt > $testdir/$x\$
  if [ $? -ne 0 ];then
    echo "run wrong!"
    nerr=$((nerr+1))
    continue
  fi
  if [ -f $testdir/$x.out ];then
    cmp $testdir/$x.out $testdir/$x\$
    if [ $? -ne 0 ];then
      echo "error detected file is $x"
    nerr=$((nerr+1))
    fi
  fi
done

if [ $nerr -ne 0 ];then
  echo "errors $nerr"
else
  echo "pass all!"
  rm $testdir/*.\$
fi



