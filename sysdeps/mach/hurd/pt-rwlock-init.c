/* Copyright (C) 2016 Free Software Foundation, Inc.
   Contributed by Agustina Arzille <avarzille@riseup.net>, 2016.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License
   as published by the Free Software Foundation; either
   version 2 of the license, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this program; if not, see
   <http://www.gnu.org/licenses/>.
*/

#include <pthread.h>
#include <pt-internal.h>
#include <bits/pt-atomic.h>
#include <hurdlock.h>
#include "pt-rwlock.h"

static const pthread_rwlockattr_t dfl_attr =
{
  .__pshared = PTHREAD_PROCESS_PRIVATE
};

int _pthread_rwlock_init (pthread_rwlock_t *rwp,
  const pthread_rwlockattr_t *attrp)
{
  if (!attrp)
    attrp = &dfl_attr;

  rwp->__shpid_qwr.__qv = rwp->__oid_nrd.__qv = 0;
  rwp->__flags = attrp->__pshared == PTHREAD_PROCESS_SHARED ?
    GSYNC_SHARED : 0;

  return (0);
}

strong_alias (_pthread_rwlock_init, pthread_rwlock_init)

