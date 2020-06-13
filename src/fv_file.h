/*
    fv: A simple terminal based file viewer.
    Copyright (C) 2020  Sai Varshith

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Author Email: hosvarshith@gmail.com
*/

#ifndef _FILE_H_
#define _FILE_H_

#define _GNU_SOURCE
#include <stdio.h>

#define ROW_STEP 64

struct filerow {
    char *line;
    size_t len;
};

struct fv_file {
    char *filename;             /* name of the file */
    int filename_len;           /* strlen() of the filename */
    FILE *fptr;                 /* file pointer */
    int line_count;             /* total number of lines in the file */
    struct filerow **contents;  /* dynamic array of filerows */
    int line_capacity;          /* current capacity of the contents array */
};

struct fv_file *handle_file(char *filename);

#endif /* _FV_H_ */
