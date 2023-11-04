#!/usr/bin/env bash

for clientNum in 1 2 3 4
do
gcc -o client$clientNum client$clientNum.c
done
