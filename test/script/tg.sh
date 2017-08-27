#!/bin/sh
mysrc="parser tknzr"
for x in $mysrc;do
  ctags $x/*
  cd $x
  ctags ./*
  cd ..
done
