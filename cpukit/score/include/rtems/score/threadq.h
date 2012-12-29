/**
 *  @file  rtems/score/threadq.h
 *
 *  Constants and Structures Associated with the Manipulation of Objects
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of objects.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREADQ_H
#define _RTEMS_SCORE_THREADQ_H

/**
 *  @defgroup ScoreThreadQ Thread Queue Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality related to managing sets of threads
 *  blocked waiting for resources.
 */
/**@{*/

#include <rtems/score/tqdata.h>

#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Constant for indefinite wait.
 */
#define THREAD_QUEUE_WAIT_FOREVER  WATCHDOG_NO_TIMEOUT

/**
 *  The following type defines the callout used when a remote task
 *  is extracted from a local thread queue.
 */
typedef void ( *Thread_queue_Flush_callout )(
                  Thread_Control *
             );

/**
 *  The following type defines the callout used for timeout processing
 *  methods.
 */
typedef void ( *Thread_queue_Timeout_callout )(
                 Objects_Id,
                 void *
             );

/**
 *  @brief Gets a pointer to a thread waiting on the_thread_queue.
 *
 *  This function returns a pointer to a thread waiting on
 *  the_thread_queue.  The selection of this thread is based on
 *  the discipline of the_thread_queue.  If no threads are waiting
 *  on the_thread_queue, then NULL is returned.
 */
Thread_Control *_Thread_queue_Dequeue(
  Thread_queue_Control *the_thread_queue
);

/**
 *  @brief Enqueues the currently executing thread on the_thread_queue.
 *
 *  This routine enqueues the currently executing thread on
 *  the_thread_queue with an optional timeout.
 */
#define _Thread_queue_Enqueue( _the_thread_queue, _timeout ) \
  _Thread_queue_Enqueue_with_handler( \
    _the_thread_queue, \
    _timeout, \
    _Thread_queue_Timeout )

/**
 *  @brief Blocks a thread and places it on a thread.
 *
 *  This routine blocks a thread, places it on a thread, and optionally
 *  starts a timeout timer.
 *
 *  @param[in] the_thread_queue pointer to threadq
 *  @param[in] timeout interval to wait
 *
 *  - INTERRUPT LATENCY:
 *    + single case
 */
void _Thread_queue_Enqueue_with_handler(
  Thread_queue_Control*        the_thread_queue,
  Watchdog_Interval            timeout,
  Thread_queue_Timeout_callout handler
);

/**
 *  @brief Invoked when a thread changes priority and is blocked.
 *
 *  This routine is invoked when a thread changes priority and is
 *  blocked on a thread queue.  If the queue is priority ordered,
 *  the_thread is removed from the_thread_queue and reinserted using
 *  its new priority.  This method has no impact on the state of the_thread
 *  or of any timeouts associated with this blocking.
 *
 *  @param[in] the_thread_queue pointer to a threadq header
 *  @param[in] the_thread pointer to a thread control block
 */
void _Thread_queue_Requeue(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
);

/**
 *  @brief Extracts thread from thread queue.
 *
 *  This routine removes @a the_thread from @a the_thread_queue
 *  and cancels any timeouts associated with this blocking.
 *
 *  @param[in] the_thread_queue is the pointer to the ThreadQ header
 *  @param[in] the_thread is the pointer to a thread control block that is to be removed
 */
void _Thread_queue_Extract(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
);

/**
 *  @brief Extracts the_thread from the_thread_queue.
 *
 *  This routine extracts the_thread from the_thread_queue
 *  and ensures that if there is a proxy for this task on
 *  another node, it is also dealt with.
 */
bool _Thread_queue_Extract_with_proxy(
  Thread_Control       *the_thread
);

/**
 *  @brief Gets a pointer to the "first" thread on the_thread_queue.
 *
 *  This function returns a pointer to the "first" thread
 *  on the_thread_queue.  The "first" thread is selected
 *  based on the discipline of the_thread_queue.
 *
 *  @param[in] the_thread_queue pointer to thread queue
 *
 *  @retval first thread or NULL
 */
Thread_Control *_Thread_queue_First(
  Thread_queue_Control *the_thread_queue
);

/**
 *  @brief Unblocks all threads blocked on the_thread_queue.
 *
 *  This routine unblocks all threads blocked on the_thread_queue
 *  and cancels any associated timeouts.
 */
void _Thread_queue_Flush(
  Thread_queue_Control       *the_thread_queue,
  Thread_queue_Flush_callout  remote_extract_callout,
  uint32_t                    status
);

/**
 *  @brief Initialize the_thread_queue.
 *
 *  This routine initializes the_thread_queue based on the
 *  discipline indicated in attribute_set.  The state set on
 *  threads which block on the_thread_queue is state.
 *
 *  @param[in] the_thread_queue is the pointer to a threadq header
 *  @param[in] discipline is the queueing discipline
 *  @param[in] state is the state of waiting threads
 *  @param[in] timeout_status is the return on a timeout
 */
void _Thread_queue_Initialize(
  Thread_queue_Control         *the_thread_queue,
  Thread_queue_Disciplines      the_discipline,
  States_Control                state,
  uint32_t                      timeout_status
);

/**
 *  @brief Removes a thread from the specified PRIORITY based
 *  threadq, unblocks it, and cancels its timeout timer.
 *
 *  This routine removes a thread from the specified PRIORITY based
 *  threadq, unblocks it, and cancels its timeout timer.
 *
 *  - INTERRUPT LATENCY:
 *    + single case
 *
 * @param[in] the_thread_queue is a pointer to a thread queue
 *
 * @retval thread dequeued
 * @retval NULL if no thread are waiting on the_thread_queue
 */
Thread_Control *_Thread_queue_Dequeue_priority(
  Thread_queue_Control *the_thread_queue
);

/**
 *  @brief Enqueues the currently executing thread on the_thread_queue.
 *
 *  This routine enqueues the currently executing thread on
 *  the_thread_queue with an optional timeout using the
 *  priority discipline.
 *
 *  @param[in] the_thread_queue is the pointer to threadq
 *  @param[in] thread is the thread to insert
 *
 *  - INTERRUPT LATENCY:
 *    + forward less than
 *    + forward equal
 */
Thread_blocking_operation_States _Thread_queue_Enqueue_priority (
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  ISR_Level            *level_p
);

/**
 *  @brief Removes the_thread and cancels related timeouts.
 *
 *  This routine removes the_thread from the_thread_queue
 *  and cancels any timeouts associated with this blocking.
 *  @param[in] the_thread_queue pointer to a threadq header
 *  @param[in] the_thread pointer to a thread control block
 *  @param[in] requeuing true if requeuing and should not alter
 *         timeout or state
 *  - INTERRUPT LATENCY:
 *    + EXTRACT_PRIORITY
 */
void _Thread_queue_Extract_priority_helper(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  bool                  requeuing
);

/**
 *  @brief Wraps the underlying call and hides the requeuing argument.
 *
 * This macro wraps the underlying call and hides the requeuing argument.
 */

#define _Thread_queue_Extract_priority( _the_thread_queue, _the_thread ) \
  _Thread_queue_Extract_priority_helper( _the_thread_queue, _the_thread, false )
/**
 *  @brief Get highest priority thread on the_thread_queue.
 *
 *  This function returns a pointer to the "first" thread
 *  on @a the_thread_queue.  The "first" thread is the highest
 *  priority thread waiting on @a the_thread_queue.
 *
 *  @param[in] the_thread_queue is the pointer to the thread queue
 *  @retval first thread or NULL
 */
Thread_Control *_Thread_queue_First_priority(
  Thread_queue_Control *the_thread_queue
);

/**
 *  @brief Gets a pointer to the thread which has been waiting the longest.
 *
 *  This function returns a pointer to the thread which has
 *  been waiting the longest on  the_thread_queue.  If no
 *  threads are waiting on the_thread_queue, then NULL is returned.
 *
 *  @param[in] the_thread_queue is the pointer to threadq
 *
 *  @retval thread dequeued or NULL
 *
 *  - INTERRUPT LATENCY:
 *    + check sync
 *    + FIFO
 */
Thread_Control *_Thread_queue_Dequeue_fifo(
  Thread_queue_Control *the_thread_queue
);

/**
 *  @brief Enqueues the currently executing thread on the_thread_queue.
 *
 *  This routine enqueues the currently executing thread on
 *  the_thread_queue with an optional timeout using the
 *  FIFO discipline.
 *
 *    @param[in] the_thread_queue pointer to threadq
 *    @param[in] the_thread pointer to the thread to block
 *    @param[in] level_p interrupt level in case the operation blocks actually
 *
 *  - INTERRUPT LATENCY:
 *    + single case
 */
Thread_blocking_operation_States _Thread_queue_Enqueue_fifo (
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  ISR_Level            *level_p
);

/**
 *  @brief Removes the_thread from the_thread_queue and cancels any timeouts.
 *
 *  This routine removes the_thread from the_thread_queue
 *  and cancels any timeouts associated with this blocking.
 */
void _Thread_queue_Extract_fifo(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
);

/**
 *  @brief Gets a pointer to the "first" thread on the_thread_queue.
 *
 *  This function returns a pointer to the "first" thread
 *  on the_thread_queue.  The first thread is the thread
 *  which has been waiting longest on the_thread_queue.
 *
 *  @param[in] the_thread_queue is the pointer to threadq
 *
 *  @retval first thread or NULL
 */
Thread_Control *_Thread_queue_First_fifo(
  Thread_queue_Control *the_thread_queue
);

/**
 *  @brief Thread queue timeout.
 *
 *  This routine is invoked when a task's request has not
 *  been satisfied after the timeout interval specified to
 *  enqueue.  The task represented by ID will be unblocked and
 *  its status code will be set in it's control block to indicate
 *  that a timeout has occurred.
 *
 *  @param[in] id thread id
 */
void _Thread_queue_Timeout (
  Objects_Id  id,
  void       *ignored
);

/**
 *  @brief Process thread queue timeout.
 *
 * This is a shared helper routine which makes it easier to have multiple
 * object class specific timeout routines.
 *
 * @param[in] the_thread is the thread to extract
 *
 * @note This method assumes thread dispatching is disabled
 *       and is expected to be called via the processing of
 *       a clock tick.
 */
void _Thread_queue_Process_timeout(
  Thread_Control *the_thread
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/threadq.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */