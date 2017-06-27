#!/bin/sh

TESTFILE="string_literal char_literal comment id keywords float_literal integer_literal operators "

testdir="test"
testbin="./bin/tokenizer"
nerr=0

echo build the tokenizer 
cd bin;cmake .. -Ddebug=0 && make
if [ $? -ne 0 ];then
  echo make failed!
  exit 1
fi
cd ..


for x in $TESTFILE;do
  echo testing $x
  $testbin $testdir/$x.txt > $testdir/$x\$
  if [ $? -ne 0 ];then
    echo "$x run wrong!"
    nerr=$((nerr+1))
    continue
  fi
done

if [ $nerr -ne 0 ];then
  echo "errors $nerr"
else
  echo "pass all!"
  rm $testdir/*\$
fi



