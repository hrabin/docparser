#!/bin/bash

FILENAME="doc"

if [ ! -f ./docparse ]; then
	echo "Parser not found!"
	gcc -o docparse docparse.c
fi

if [ ! -f ./docparse ]; then
	echo "ERROR: cant create parser"
	exit 0
fi

./build_tex.sh > ./$FILENAME.tex

# build PDF
pdflatex $FILENAME.tex
# repeat, to have TOC generated correctly
pdflatex $FILENAME.tex

