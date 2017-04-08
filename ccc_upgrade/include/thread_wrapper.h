/*
 * thread_wrapper.h
 *
 *  Created on: 2014Äê1ÔÂ1ÈÕ
 *      Author: flx
 */

#ifndef __THREAD_WRAPPER_H__
#define __THREAD_WRAPPER_H__

#include <pthread.h>

extern int  thread_create(void *(*func)(void*), void *arg);
extern void thread_exit(void *status);

#endif /* __THREAD_WRAPPER_H__ */
