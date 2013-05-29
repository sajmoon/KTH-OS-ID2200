#!/bin/bash
if [ -x a.out ]
then
  rm a.out
fi

gcc -DSTRATEGY=2 -pg -Wall testPrestanda.c malloc.c
if [ $? -eq 0 ]
then
  ./a.out
else
  echo "Compilation failed"
fi

