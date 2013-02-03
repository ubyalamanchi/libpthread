/* Wait for thread termination.
   Copyright (C) 2000 Free Software Foundation, Inc.
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

#include <errno.h>
#include <pthread.h>
#include <stddef.h>

#include <pt-internal.h>

/* Make calling thread wait for termination of thread THREAD.  Return
   the exit status of the thread in *STATUS.  */
int
pthread_join (pthread_t thread, void **status)
{
  struct __pthread *pthread;
  int err = 0;

  /* Lookup the thread structure for THREAD.  */
  pthread = __pthread_getid (thread);
  if (pthread == NULL)
    return ESRCH;

  __pthread_mutex_lock (&pthread->state_lock);
  pthread_cleanup_push ((void (*)(void *)) __pthread_mutex_unlock,
			&pthread->state_lock);

  /* Rely on pthread_cond_wait being a cancellation point to make
     pthread_join one too.  */
  while (pthread->state == PTHREAD_JOINABLE)
    pthread_cond_wait (&pthread->state_cond, &pthread->state_lock);

  pthread_cleanup_pop (0);

  switch (pthread->state)
    {
    case PTHREAD_EXITED:
      __pthread_mutex_unlock (&pthread->state_lock);

      /* THREAD has already exited.  Salvage its exit status.  */
      if (status)
	*status = pthread->status;

      /* Make sure the thread is not running before we remove its
         stack.  (The only possibility is that it is in a call to
         __pthread_thread_halt itself, but that is enough to cause a
         sigsegv.)  */
      __pthread_thread_halt (pthread);

      /* Destroy the stack, the kernel resources and the control
	 block.  */
      assert (pthread->stack);
      __pthread_stack_dealloc (pthread->stackaddr, pthread->stacksize);
      pthread->stack = 0;

      __pthread_thread_dealloc (pthread);

      __pthread_dealloc (pthread);
      break;

    case PTHREAD_TERMINATED:
      /* Pretend THREAD wasn't there in the first place.  */
      __pthread_mutex_unlock (&pthread->state_lock);
      err = ESRCH;
      break;

    default:
      /* Thou shalt not join non-joinable threads!  */
      __pthread_mutex_unlock (&pthread->state_lock);
      err = EINVAL;
      break;
    }

  return err;
}
