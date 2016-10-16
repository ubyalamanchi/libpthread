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

int __pthread_rwlock_wrlock (pthread_rwlock_t *rwp)
{
  int flags = rwp->__flags & GSYNC_SHARED;
  unsigned int self_id = early_pthread_self();

  if (rwl_owned_p (rwp, self_id, flags))
    return (EDEADLK);

  while (1)
    {
      unsigned int *ptr = &rwl_oid (rwp->__oid_nrd);
      atomic_read_barrier ();
      unsigned int owner = *ptr;

      if (owner == RWLOCK_UNOWNED)
        {
          if (atomic_compare_and_exchange_bool_acq (ptr, self_id, owner) == 0)
            {
              rwl_setown (rwp, flags);
              return (0);
            }
        }
      else
        {
          /* Wait on the address. We are only interested in the value of
           * the OID field, but we need a different queue for writers.
           * As such, we use 64-bit values, with the high word being
           * the owner id. */
          unsigned int nw = atomic_exchange_and_add (--ptr, 1);
          lll_xwait (ptr, nw + 1, owner, flags);
          atomic_decrement (ptr);
        }
    }
}

strong_alias (__pthread_rwlock_wrlock, pthread_rwlock_wrlock)
