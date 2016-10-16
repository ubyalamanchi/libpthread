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

#ifndef __HURD_XINT_H__
#define __HURD_XINT_H__   1

#include <bits/wordsize.h>

#if __WORDSIZE == 64
typedef unsigned long int __hurd_xint64;
#else
typedef unsigned long long int __hurd_xint64;
#endif

/* 64-bit integer that allows direct access to its low
 * and high limbs. */
union __hurd_xint
{
  __hurd_xint64 __qv;
  struct
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
      unsigned long __hi;
      unsigned long __lo;
#  define __hurd_xint_pair(lo, hi)   hi, lo
#else
#  define __hurd_xint_pair(lo, hi)   lo, hi
      unsigned long __lo;
      unsigned long __hi;
#endif
    };
};

#endif
