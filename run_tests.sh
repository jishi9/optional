#!/bin/bash

mkdir -p output &&
  clang++ -std=c++11 test_optional.cc -o output/run_tests &&
  output/run_tests
