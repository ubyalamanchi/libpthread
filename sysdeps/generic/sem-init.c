/* Initialize a semaphore.  Generic version.
   Copyright (C) 2005 Free Software Foundation, Inc.
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

#include <semaphore.h>
#include <errno.h>

#include <pt-internal.h>

int
__sem_init (sem_t *sem, int pshared, unsigned value)
{
  if (pshared != 0)
    {
      errno = EOPNOTSUPP;
      return -1;
    }

#ifdef SEM_VALUE_MAX
  if (value > SEM_VALUE_MAX)
    {
      errno = EINVAL;
      return -1;
    }
#endif

  *sem = (sem_t) __SEMAPHORE_INITIALIZER (pshared, value);
  return 0;
}

weak_alias (__sem_init, sem_init);
