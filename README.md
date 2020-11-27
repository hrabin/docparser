# docparser

This is parser for inline documentation, primary for C an C++ documentation. 
Unlike i.e. Doxygene, this is not for code documenatation but user documentation.
Basic format mark is tripple-slash ('DOC MARK' = '///') and rest of line will be used for generating LATEX documentation.
Basic function is to copy text after mark into latex document with additional text processing.

It will build its own PDF documentation using 'makedoc.sh'.

See 'doc.pdf' which is generated from this source codes.

# requirements

Linux-like OS.
Installed gcc and pdflatex.

