/* Deallocate the kernel thread resources.  Mach version.
   Copyright (C) 2000,02 Free Software Foundation, Inc.
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

#include <assert.h>
#include <errno.h>
#include <mach.h>

#include <pt-internal.h>

extern L4_ThreadId_t __task_server;

/* Deallocate the kernel thread resources associated with THREAD.  */
void
__pthread_thread_halt (struct __pthread *thread)
{
  CORBA_Environment env = idl4_default_environment;
  L4_Word_t *t = (L4_Word_t *) &thread->threadid;

  assert (*t);
  assert (thread_terminate (__task_server, *t, &env));
  *t = 0;
}
