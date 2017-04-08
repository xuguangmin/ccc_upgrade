/*
 * util_log.h
 *
 *  Created on: 2012-11-1
 *      Author: flx
 */

#ifndef __UTIL_LOG_H__
#define __UTIL_LOG_H__

#include <stdio.h>
#include <syslog.h>
#include "ccc_config.h"


extern void ccc_log(const char *format, ...);


#ifdef CCC_UPGRADE_DEBUG
#define CCC_LOG_OUT(fmt, args...) \
		printf(fmt, ##args);
#else
#define CCC_LOG_OUT(fmt, args...) \
		syslog(LOG_INFO, fmt, ##args);
#endif


#endif /* __UTIL_LOG_H__ */
