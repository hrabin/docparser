#!/bin/bash
# TEX source generator script

MAIN_DIR="./"

FILELIST="$MAIN_DIR/docparse.c"

REFERENCE="refs.txt"


echo '

\documentclass[oneside,a4paper,10pt]{article}
\usepackage[english]{babel} % LC_ALL="" pdflatex file.tex
\usepackage[utf8]{inputenc}
\usepackage[left=20mm, bottom=20mm, right=20mm, includefoot]{geometry}
\usepackage{framed}
\usepackage{color}
\usepackage{graphicx}

\usepackage{enumerate}
\usepackage{xcolor}
\usepackage{hyperref}
\hypersetup{colorlinks=false,linkbordercolor=blue, linkcolor=green,pdfborderstyle={/S/U/W 1}}

%\definecolor{shadecolor}{rgb}{0.8,0.8,0.8}

\usepackage{graphicx}
\usepackage{fancyhdr}
\usepackage{geometry}

\geometry{headheight = 0.6in}
\fancypagestyle{plain}
{
	\fancyhf{}
%	\fancyhead[L]{\includegraphics[height=0.5in, keepaspectratio=true]{logo.pdf}}
	\fancyhead[R]{
		My company name\\
		Addres of my company\\
		\href{mailto:mail@example.com}{mail@example.com}
	}
	\cfoot{\thepage}
}

\pagestyle{plain}

\begin{document}

\title{Docparser: inline docomentation system}
\author{GMMultimedia.cz}

\begin{center}
\today 
\end{center}

\tableofcontents
\pagebreak

\section{About}
This document is automaticaly generated from its own source codes. 

'

# build references for later including into document
# in source it is : /*~r<N>*/"some text"/*~*/ where <N> is number or name of reference
echo -n "" > $REFERENCE

for file in $FILELIST; do 
cat $file | grep -o "/\*~r.*\*~\*/" >> $REFERENCE
done

for file in $FILELIST; do 
cat $file | ./docparse -f $REFERENCE
done

echo "\end{document}"

exit 1

