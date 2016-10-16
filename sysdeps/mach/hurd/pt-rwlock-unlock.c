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

int __pthread_rwlock_unlock (pthread_rwlock_t *rwp)
{
  int flags = rwp->__flags & GSYNC_SHARED;
  atomic_read_barrier ();
  unsigned int owner = rwl_oid (rwp->__oid_nrd);

  if ((owner & ID_MASK) != 0)
    {
      /* A writer holds the lock. */
      if (!rwl_owned_p (rwp, early_pthread_self(), flags))
        /* ... But it isn't us. */
        return (EPERM);

      rwl_spid(rwp) = 0;
      rwl_oid(rwp->__oid_nrd) = RWLOCK_UNOWNED;
      atomic_write_barrier ();

      /* The exclusive lock has been released. Now decide whether
       * to wake a queued writer (preferred), or all the queued readers. */
      if (rwl_qwr (rwp->__oid_nrd) > 0)
        lll_wake (&rwl_qwr(rwp->__oid_nrd), flags);
      else if (rwl_nrd (rwp->__oid_nrd) > 0)
        lll_wake (&rwl_oid(rwp->__oid_nrd), flags | GSYNC_BROADCAST);
    }
  else if (rwl_nrd (rwp->__oid_nrd) == 0)
    return (EPERM);
  else
    {
      union __hurd_xint tmp;
      while (1)
        {
          tmp.__qv = atomic_loadx (&rwp->__oid_nrd.__qv);
          if (catomic_casx_bool (&rwp->__oid_nrd.__qv,
              hurd_xint_pair (tmp.__lo, tmp.__hi),
              hurd_xint_pair (rwl_nrd (tmp) == 1 ?
                RWLOCK_UNOWNED : RWLOCK_RO, rwl_nrd (tmp) - 1)))
            break;
        }

      /* As a reader, we only need to do a wakeup iff:
       * - We were the last one.
       * - There's at least a writer queued. */
      if (rwl_nrd (tmp) == 1 && rwl_qwr (rwp->__oid_nrd) > 0)
        lll_wake (&rwl_qwr(rwp->__oid_nrd), flags);
    }

  return (0);
}

strong_alias (__pthread_rwlock_unlock, pthread_rwlock_unlock)
