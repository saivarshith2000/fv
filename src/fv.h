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

#ifndef _FV_H_
#define _FV_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>

/* A simple exit macro which displays an error message and exits */
#define DIE(msg) {\
    fprintf(stderr, "ERROR: %s\n", msg);\
    fprintf(stderr, "AT: %d in %s\n", __LINE__, __FILE__);\
    fprintf(stderr, "ERRNO: %d (%s)", errno, strerror(errno)); \
    exit(EXIT_FAILURE);}

#endif /* _FV_H_ */
