/* Atomic operations.  gcc intrinsics version.
   Copyright (C) 2016 Free Software Foundation, Inc.
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

#ifndef _BITS_ATOMIC_H
#define _BITS_ATOMIC_H	1

typedef __volatile int __atomic_t;

static inline void
__atomic_inc (__atomic_t *__var)
{
  __atomic_add_fetch (__var, 1, __ATOMIC_SEQ_CST);
}

static inline void
__atomic_dec (__atomic_t *__var)
{
  __atomic_add_fetch (__var, -1, __ATOMIC_SEQ_CST);
}

static inline int
__atomic_dec_and_test (__atomic_t *__var)
{
  __atomic_t res = __atomic_add_fetch (__var, -1, __ATOMIC_SEQ_CST);
  return res == 0;
}

#define atomic_loadx(p) __atomic_load_8(p, __ATOMIC_SEQ_CST)
#define atomic_storex(p, v) __atomic_store_8(p, v, __ATOMIC_SEQ_CST)

static inline char atomic_casx_bool(uint64_t *p,
    uint32_t elo, uint32_t ehi, uint32_t nlo, uint32_t nhi)
{
  uint64_t n = nlo | (((uint64_t) nhi) << 32);
  uint64_t e = elo | (((uint64_t) ehi) << 32);
  return __atomic_compare_exchange(p,
    &e, &n, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

#endif
