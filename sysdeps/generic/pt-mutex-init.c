/* Initialize a mutex.  Generic version.
   Copyright (C) 2000,02 Free Software Foundation, Inc.
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

#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <pt-internal.h>

int
_pthread_mutex_init (pthread_mutex_t *mutex,
		     const pthread_mutexattr_t *attr)
{
  *mutex = __PTHREAD_MUTEX_INITIALIZER;

  if (! attr
      || memcmp (attr, &__pthread_default_mutexattr, sizeof (*attr) == 0))
    /* Use the default attributes.  */
    return 0;

  /* Non-default attributes.  */

  mutex->attr = malloc (sizeof *attr);
  if (! mutex->attr)
    return ENOMEM;

  *mutex->attr = *attr;
  return 0;
}

weak_alias (_pthread_mutex_init, pthread_mutex_init);
