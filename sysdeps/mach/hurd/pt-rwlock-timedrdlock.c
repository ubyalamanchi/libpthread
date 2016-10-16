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

int pthread_rwlock_timedrdlock (pthread_rwlock_t *rwp,
  const struct timespec *abstime)
{
  int flags = rwp->__flags & GSYNC_SHARED;

  if (rwl_owned_p (rwp, early_pthread_self(), flags))
    return (EDEADLK);

  while (1)
    {
      union __hurd_xint tmp = { atomic_loadx (&rwp->__oid_nrd.__qv) };
      if ((rwl_oid (tmp) & ID_MASK) == 0)
        {
          if (catomic_casx_bool (&rwp->__oid_nrd.__qv,
              hurd_xint_pair (tmp.__lo, tmp.__hi),
              hurd_xint_pair (RWLOCK_RO, rwl_nrd (tmp) + 1)))
            {
              if (rwl_oid (tmp) != RWLOCK_RO && rwl_nrd (tmp) > 0)
                lll_wake (&rwl_oid(rwp->__oid_nrd), flags | GSYNC_BROADCAST);

              return (0);
            }
        }
      else
        {
          /* The timeout parameter has to be checked on every iteration,
           * because its value may not be examined if the lock can be
           * taken without blocking. */

          if (__glibc_unlikely (abstime->tv_nsec < 0 ||
              abstime->tv_nsec >= 1000000000))
            return (EINVAL);

          atomic_increment (&rwl_nrd(rwp->__oid_nrd));
          int ret = lll_abstimed_wait (&rwl_oid(rwp->__oid_nrd),
            rwl_oid (tmp), abstime, flags);
          atomic_decrement (&rwl_nrd(rwp->__oid_nrd));

          if (ret == KERN_TIMEDOUT)
            return (ETIMEDOUT);
        }
    }
}
