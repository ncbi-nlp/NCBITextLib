# NCBITextLib #

NCBITextLib is a simple but effective software library that allows one
to build and access an infrastructure for large-scale text mining tasks.
This library only provides basic C++ classes for building various text
mining tools. Since the library provides a simple to use interface for
connecting an internal text data structure to other high-level
applications, it is straightforward to build ML software upon
NCBITextLib. Currently, we provide three machine learning classes (naive
Bayes, support vector machine and theme analysis algorithms) and example
codes that use NCBITextLib.

## Public Domain Notice ##

This work is a "United States Government Work" under the terms of the
United States Copyright Act. It was written as part of the authors'
official duties as a United States Government employee and thus cannot
be copyrighted within the United States. The data is freely available
to the public for use. The National Library of Medicine and the U.S.
Government have not placed any restriction on its use or reproduction.

Although all reasonable efforts have been taken to ensure the accuracy
and reliability of the data and its source code, the NLM and the
U.S. Government do not and cannot warrant the performance or results
that may be obtained by using it. The NLM and the U.S. Government
disclaim all warranties, express or implied, including warranties of
performance, merchantability or fitness for any particular purpose.

## Tested System ##

- g++ (gcc) 4.8.1
- OS: CentOS release 6.7

## How to Use ##

1. Building a library
  * cd ./lib
  * make

2. Compiling example programs
  * cd ./applications
  * make [file name], e.g. make make_db

3. Machine learing classes and examples
  * cd ./applications
  * BayeX.h: naive Bayes classifier (inherit from CMark)
  * HubeX.h: support vector machine classifier (inherit from CMark)
  * ThemX.h: theme analysis algorithm (inherit from BayeX)
  * make_doc: create a Doc from samples.txt
  * make_xpost: create a XPost from a Doc set
  * run_BayeX: naive Bayes classifier example
  * run_HubeX: support vector machine classifier example
  * run_ThemX: theme analysis algorithm example
  * find_neighbors: find neighboring documents from a seed document

## List of Contributors ##

- Sun Kim
- W. John Wilbur
- Won Kim
- Donald C. Comeau
- Zhiyong Lu

## Contact ##

Please contact sun.kim@nih.gov if you have any questions or comments.
