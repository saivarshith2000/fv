# fv - a command line file pager.
![build](https://github.com/saivarshith2000/fv/workflows/build/badge.svg)
[![GitHub license](https://img.shields.io/github/license/saivarshith2000/fv)](https://github.com/saivarshith2000/fv/blob/master/LICENSE)

fv is a simple command line file pager written as a learning exercise.

## Building
You'll need a C compiler to build fv.
```
git clone https://github.com/saivarshith2000/fv.git
cd fv && make
```

## Install
After building fv, you can install it with ``` make install ```. By default fv is installed
to ``` /usr/local/bin/ ``` and the man page is installed to ``` /usr/local/man/man1/ ```. To modify these variables
you can tweak the Makefile.

**Note: fv is tested on linux systems only. It will NOT work on windows.**

## Usage
```
fv <filename>[:<line-number>] [-l] [-w] [-h] [-v]

    <filename>[:<line-number>]
        Open file <filename>. To open the file at a specific line append ':' and the line-number to filename.
    -l disable line numbers.
    -f disable line folding.
    -h show usage.
    -v print version.
```

## Keybindings
```
q - exit fv
h - scroll left a column
j - scroll down a row
k - scroll up a row
l - scroll right a column.
g - scroll to top of the file
G - scroll to bottom of the file
$ - scroll to end of the largest line
^ - scroll to start of line
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
