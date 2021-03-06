/* Mutex type.  Generic version.
   Copyright (C) 2000-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If
   not, see <http://www.gnu.org/licenses/>.  */

#ifndef _BITS_MUTEX_H
#define _BITS_MUTEX_H	1

#include <bits/mutex-attr.h>

/* User visible part of a mutex.  */
struct __pthread_mutex
{
  unsigned int __lock;
  unsigned int __owner_id;
  unsigned int __cnt;
  int __shpid;
  int __type;
  int __flags;
  unsigned int __reserved1;
  unsigned int __reserved2;
};

/* Static mutex initializers. */
#define __PTHREAD_MUTEX_INITIALIZER   \
  { 0, 0, 0, 0, __PTHREAD_MUTEX_TIMED, 0, 0, 0 }

/* The +1 is to mantain binary compatibility with the old
 * libpthread implementation. */
#define __PTHREAD_ERRORCHECK_MUTEX_INITIALIZER   \
  { 0, 0, 0, 0, __PTHREAD_MUTEX_ERRORCHECK + 1, 0, 0, 0 }

#define __PTHREAD_RECURSIVE_MUTEX_INITIALIZER   \
  { 0, 0, 0, 0, __PTHREAD_MUTEX_RECURSIVE + 1, 0, 0, 0 }

#endif /* bits/mutex.h */
