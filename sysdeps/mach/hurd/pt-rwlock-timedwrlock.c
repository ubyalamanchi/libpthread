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

int pthread_rwlock_timedwrlock (pthread_rwlock_t *rwp,
  const struct timespec *abstime)
{
  unsigned int self_id = early_pthread_self();
  int flags = rwp->__flags & GSYNC_SHARED;

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
          if (__glibc_unlikely (abstime->tv_nsec < 0 ||
              abstime->tv_nsec >= 1000000000))
            return (EINVAL);

          unsigned int nw = atomic_exchange_and_add (--ptr, 1);
          int ret = lll_abstimed_xwait (ptr, nw + 1, owner, abstime, flags);
          nw = atomic_exchange_and_add (ptr, -1);

          if (ret == KERN_TIMEDOUT)
            {
              /* If we timed out, there are no writers pending, the
               * lock is unowned *and* there are readers blocked, it's
               * possible that a wakeup was meant for us, but we timed
               * out first. In such unlikely case, we wake every reader
               * in order to avoid a potential deadlock. */

              union __hurd_xint tmp = { atomic_loadx (&rwp->__oid_nrd.__qv) };
              if (__glibc_unlikely (nw == 1 && rwl_nrd (tmp) > 0 &&
                  rwl_oid (tmp) == RWLOCK_UNOWNED))
                lll_wake (&rwl_oid(rwp->__oid_nrd), flags | GSYNC_BROADCAST);

              /* We still return with an error. */
              return (ETIMEDOUT);
            }
        }
    }
}
