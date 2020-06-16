# fv - a command line file pager.
[![Build Status](https://travis-ci.com/saivarshith2000/fv.svg?branch=master)](https://travis-ci.com/saivarshith2000/fv)
[![GitHub license](https://img.shields.io/github/license/saivarshith2000/fv)](https://github.com/saivarshith2000/fv/blob/master/LICENSE)  

fv is a simple command line file pager written as a learning exercise.

## Installation
```
1. You'll need a C compiler (the makefile uses cc.)
1. git clone https://github.com/saivarshith2000/fv.git
2. cd fv && make install
```
Note: fv is tested on linux systems only. It will not work on windows.

## Usage
```
fv <filename>[:<line-number>] [-l] [-w] [-h] [-v]

    <filename>[:<line-number>]
        Open file <filename>. To open the file at a specific line append ':' and the line-number to filename.
    -l disable line numbers.
    -f enable line folding.
    -h show usage.
    -v print version.
```

## Keybindings
```
h,j,k,l - scroll left, down, up, right by one row/column.
q - exit fv.
g - goto top of the file
G - goto bottom of the file
n - move to next occurance of search term
N - move to previous occurance of search term

<num><key>RETURN - equivalent to pressing <key> <num> times where <key> is one of h,j,k,l.

/<string>RETURN  - search for all occurances of <string> in the file.  
                   The number of such occurances is shown in status bar.
```

Note: All the above information can be accessed with ```man fv``` command after installing fv.

## Screencap
<TODO>

## LICENSE
fv is licensed under MIT License. See [LICENSE](https://github.com/saivarshith2000/fv/blob/master/LICENSE
)