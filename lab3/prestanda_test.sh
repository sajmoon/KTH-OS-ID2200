#!/bin/bash
if [ -x testFile ]
then
  rm testFile
fi

gcc -DSTRATEGY=2 -Wall -o testFile testPrestanda.c malloc.c 2> compilation.log
if [ $? -eq 0 ]
then
  
  time ./testFile >> prestanda.log
else
  echo "Compilation failed"
fi

