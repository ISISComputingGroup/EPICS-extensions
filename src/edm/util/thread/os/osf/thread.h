#ifndef __thread_h
#define __thread_h 1

#include "thr.msh"

#define THREAD_SUCCESS( stat )\
  return stat;

#define THREAD_ERROR( stat )\
  cma_thread_exit_error();

typedef void *THREAD_HANDLE;
typedef void *THREAD_LOCK_ARRAY_HANDLE;
typedef void *THREAD_LOCK_HANDLE;

#ifdef __cplusplus
#extern "C" {
#endif

/*
** prototypes
*/
int thread_init( void );

int thread_create_handle (
  THREAD_HANDLE *handle,
  void *app_data
);

int thread_create_lock_handle (
  THREAD_LOCK_HANDLE *handle
);

int thread_destroy_handle (
  THREAD_HANDLE handle
);

int thread_destroy_lock_handle (
  THREAD_LOCK_HANDLE handle
);

void *thread_get_app_data (
  THREAD_HANDLE handle
);

int thread_create_proc (
  THREAD_HANDLE handle,
  void (*proc)( void * ) );

int thread_set_proc_priority (
  THREAD_HANDLE handle,
  char *priority	/* "h","m","l" */
);

int thread_wait_til_complete (
  THREAD_HANDLE handle
);

int thread_lock (
  THREAD_LOCK_HANDLE handle
);

int thread_unlock (
  THREAD_LOCK_HANDLE handle
);

int thread_lock_master (
  THREAD_HANDLE handle
);

int thread_unlock_master (
  THREAD_HANDLE handle
);

int thread_lock_global ( void );

int thread_unlock_global ( void );

int thread_wait_for_signal (
  THREAD_HANDLE handle
);

int thread_timed_wait_for_signal (
  THREAD_HANDLE handle,
  double seconds
);

int thread_signal (
  THREAD_HANDLE handle
);

int thread_signal_from_ast (
  THREAD_HANDLE handle
);

int thread_delay (
  THREAD_HANDLE handle,
  double seconds
);

int thread_init_timer (
  THREAD_HANDLE handle,
  double seconds
);

int thread_wait_for_timer (
  THREAD_HANDLE handle
);

int thread_set_stack (
  THREAD_HANDLE handle,
  int size );

int thread_get_stack (
  THREAD_HANDLE handle,
  int *size );

int thread_set_stack_size (
  THREAD_HANDLE handle,
  int size );

int thread_get_stack_size (
  THREAD_HANDLE handle,
  int *size );

#ifdef __cplusplus
}
#endif

#endif
