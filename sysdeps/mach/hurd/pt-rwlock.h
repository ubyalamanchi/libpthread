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

#ifndef _PT_RWLOCK_H
#define _PT_RWLOCK_H

/* Read-write locks have the following memory layout:
 * 0         4            8            12           16
 * |__shpid__|__qwriters__|__owner_id__|__nreaders__|
 *
 * Whenever a thread wants to acquire either lock, it first has to check
 * the OID field. It may be unowned, owned by readers, or owner by a
 * particular writer. For reader ownership, we use a special OID that no
 * thread can ever have.
 *
 * When it comes to waiting for the lock to change ownership, we need
 * different wait queues for readers and writers. However, both of them
 * have to monitor the OID field for changes. This is where 64-bit gsync
 * comes into play: Readers will wait on the address of the OID, while
 * writers will wait on the 64-bit address that starts at NWRITERS and
 * extends to OID as well.
 *
 * This approach can cause some extra work on the writer side, but it's
 * more efficient by virtue of being lockless. As long as we have 64-bit
 * atomics, we can safely implement the POSIX read-write lock interface
 * without using any internal locks. */

#define __rwl_atp(ptr, idx)   (((unsigned int *)(ptr))[idx])

/* Access the fields described above. */
#define rwl_qwr(val)   __rwl_atp (&(val), -1)
#define rwl_oid(val)   __rwl_atp (&(val), +0)
#define rwl_nrd(val)   __rwl_atp (&(val), +1)

/* Special ID's to represent unowned and readers-owned locks. */
#define RWLOCK_UNOWNED   (0)
#define RWLOCK_RO        (1U << 31)

#define ID_MASK   ~RWLOCK_RO

/* Access the owner's PID for task-shared rwlocks. */
#define rwl_spid(rwl)   *(unsigned int *)&(rwl)->__shpid_qwr

/* Test that a read-write lock is owned by a particular thread. */
#define rwl_owned_p(rwp, tid, flags)   \
  (rwl_oid ((rwp)->__oid_nrd) == (tid) &&   \
    (((flags) & GSYNC_SHARED) == 0 ||   \
     rwl_spid (rwp) == (unsigned int)__getpid ()))

#define rwl_setown(rwp, flags)   \
  do   \
    {   \
      if ((flags) & GSYNC_SHARED)   \
        rwl_spid(rwp) = __getpid ();   \
    }   \
  while (0)

extern int __getpid (void) __attribute__ ((const));

/* We need a function, because we're using a macro that
 * expands into a list of arguments. */
static inline int
catomic_casx_bool (unsigned long long *ptr, unsigned int elo,
  unsigned int ehi, unsigned int nlo, unsigned int nhi)
{
  return (atomic_casx_bool (ptr, elo, ehi, nlo, nhi));
}

static inline pthread_t early_pthread_self(void)
{
  struct __pthread *thread = ___pthread_self;
  if (thread)
    return thread->thread;
  else
    /* Initialization time, no need to care.  */
    return 0;
}

#endif /* pt-rwlock.h */
