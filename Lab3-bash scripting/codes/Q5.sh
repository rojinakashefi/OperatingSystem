#!/bin/bash

read -p "Your shape is: " number
case $number in
	
	1)
	echo "1
22
333
4444
5555"
	;;
	
	2)
	echo "
     .	
    . .
   . . .
  . . . .
 . . . . .
. . . . . .
. . . . . .
 . . . . .
  . . . .
   . . .
    . .
     ."
	;;
	
	3)
	echo "
|_
| |_
| | |_
| | | |_
| | | | |_"
	;;
esac
