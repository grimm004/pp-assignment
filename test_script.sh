#!/bin/bash
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

#This should give no warnings or errors
make

./gameoflife < glider.txt > output.txt
if [ $? -eq 0 ]
then
  echo Test passed: Correct error code
else
  echo Test failed: Wrong error code
fi
diff  --ignore-trailing-space --ignore-blank-lines -q output.txt glider_output.txt
if [ $? -eq 0 ]
then
  echo Test passed: Correct output
else
  echo Test failed: Wrong output
fi
