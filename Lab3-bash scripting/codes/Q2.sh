#!/bin/bash

read -p "Enter number1 = " num1
read -p "which operator? (* / + -): " operand
read -p "Enter number2 = " num2

echo -n "Result is "
case $operand in
	
	/)
	echo "$(($num1 / $num2))"
	;;
		
	\*)
	echo "$(($num1 * $num2))"
	;;
	
	+)
	echo "$(($num1 + $num2))"
	;;
	
	-)
	echo "$(($num1 - $num2))"	
	;;
esac
