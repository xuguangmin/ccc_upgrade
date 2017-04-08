/*
 * util_log.c
 *
 *  Created on: 2012-11-29
 *      Author: flx
 */
#include <stdio.h>
#include <stdarg.h>

void ccc_log(const char *format, ...)
{
	va_list args;
	char buffer[1000];

	va_start(args, format);
	vsnprintf(buffer, 1000, format, args);
	va_end(args);
	//printf("%s\n", buffer);
}
