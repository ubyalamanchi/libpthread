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

int __pthread_rwlock_rdlock (pthread_rwlock_t *rwp)
{
  int flags = rwp->__flags & GSYNC_SHARED;

  /* Test that we don't own the write-lock already. */
  if (rwl_owned_p (rwp, early_pthread_self(), flags))
    return (EDEADLK);

  while (1)
    {
      union __hurd_xint tmp = { atomic_loadx (&rwp->__oid_nrd.__qv) };
      if ((rwl_oid (tmp) & ID_MASK) == 0)
        {
          /* The lock is either unowned, or the readers hold it. */
          if (catomic_casx_bool (&rwp->__oid_nrd.__qv,
              hurd_xint_pair (tmp.__lo, tmp.__hi),
              hurd_xint_pair (RWLOCK_RO, rwl_nrd (tmp) + 1)))
            {
              /* If we grabbed an unowned lock and there were readers
               * queued, notify our fellows so they stop blocking. */
              if (rwl_oid (tmp) != RWLOCK_RO && rwl_nrd (tmp) > 0)
                lll_wake (&rwl_oid(rwp->__oid_nrd), flags | GSYNC_BROADCAST);

              return (0);
            }
        }
      else
        {
          /* A writer holds the lock. Sleep. */
          atomic_increment (&rwl_nrd(rwp->__oid_nrd));
          lll_wait (&rwl_oid(rwp->__oid_nrd), rwl_oid (tmp), flags);
          atomic_decrement (&rwl_nrd(rwp->__oid_nrd));
        }
    }
}

strong_alias (__pthread_rwlock_rdlock, pthread_rwlock_rdlock)
