#! /bin/sh

vimdiff -c "set diffopt+=iwhite" -c "map <F8> :qa<CR>" output/$1.txt demoData/answer/$1.txt
