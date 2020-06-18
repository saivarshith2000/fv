# fv - a command line file pager.
![build](https://github.com/saivarshith2000/fv/workflows/build/badge.svg)
[![GitHub license](https://img.shields.io/github/license/saivarshith2000/fv)](https://github.com/saivarshith2000/fv/blob/master/LICENSE)

fv is a simple command line file pager written as a learning exercise.

## Install
```
git clone https://github.com/saivarshith2000/fv.git
cd git
make install
```
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

<num><key>RETURN - equivalent to pressing <key> <num> times where <key> is one of h,j,k,l.

Note: All the above information can be accessed with ```man fv``` command after installing fv.

## Screencap
<TODO>

## LICENSE
fv is licensed under MIT License. See [LICENSE](https://github.com/saivarshith2000/fv/blob/master/LICENSE
)
