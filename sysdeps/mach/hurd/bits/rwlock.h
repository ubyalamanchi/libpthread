/* rwlock type.  Generic version.
   Copyright (C) 2002-2016 Free Software Foundation, Inc.
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
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef _BITS_RWLOCK_H
#define _BITS_RWLOCK_H

#include <bits/xint.h>

struct __pthread_rwlock
{
  union __hurd_xint __shpid_qwr;
  union __hurd_xint __oid_nrd;
  int __flags;
  unsigned int __reserved1;
  unsigned int __reserved2;
};

/* Static initializer for read-write locks. */
#define __PTHREAD_RWLOCK_INITIALIZER    { { 0 }, { 0 }, 0 }

#endif /* bits/rwlock.h */
