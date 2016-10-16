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

int pthread_rwlock_trywrlock (pthread_rwlock_t *rwp)
{
  unsigned int self_id = early_pthread_self();
  unsigned int *ptr = &rwl_oid (rwp->__oid_nrd);
  atomic_read_barrier ();
  unsigned int owner = *ptr;

  if (rwl_owned_p (rwp, self_id, rwp->__flags))
    return (EDEADLK);
  else if (owner == RWLOCK_UNOWNED &&
      atomic_compare_and_exchange_bool_acq (ptr, self_id, owner) == 0)
    {
      rwl_setown (rwp, rwp->__flags);
      return (0);
    }

  return (EBUSY);
}
