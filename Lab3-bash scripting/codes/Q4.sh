#!/bin/bash

read -p "x: " x
read -p "y: " y
read -p "Enter name of file: " file
head -n $y $file | tail -n $(($y - $x + 1))
