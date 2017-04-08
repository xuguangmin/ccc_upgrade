/*
 * thread_wrapper.c
 *
 *  Created on: 2014Äê1ÔÂ1ÈÕ
 *      Author: flx
 */
#include <errno.h>
#include <pthread.h>

int thread_create(void *(*func)(void*), void *arg)
{
	int result;
	pthread_t pid;

	result = pthread_create(&pid, NULL, func, arg);
	pthread_detach(pid);

	return (0 == result) ? 1:0;
}

void thread_exit(void *status)
{
	pthread_exit(status);
}

