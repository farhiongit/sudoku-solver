#!/usr/bin/ksh
typeset -R3 i=0
time while IFS= read -r sudoku ; do
	((i++))
	print -n "$i $sudoku "
	./solveSudoku -q $sudoku
	print "RESULT=$?"
done < Top95.sudoku

