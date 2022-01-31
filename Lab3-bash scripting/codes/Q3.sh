#!/bin/bash

while true;
do
tmp=0
answer=0
sum=0
read -p "Enter a number: " number 
while [ ! $number -eq 0 ]
do
let tmp=number%10;
let sum=sum+tmp;
let answer=( answer*10 )+tmp;
let number=number-number%10;
let number=number/10;
done
echo "answer is $answer";
echo "sum of digits is $sum";
done
