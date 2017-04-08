/*
 * util_func.h
 *
 *  Created on: 2012-11-20
 *      Author: flx
 *
 *      一些工具类函数
 */

#ifndef __UTIL_FUNC_H__
#define __UTIL_FUNC_H__

#include <stdint.h>

typedef void (*host_ip_callback)(const uint8_t *ip, int data_len);

extern char *util_strcpy(const char *srcstr);
extern const char *util_strcasestr(const char *srcstr, const char *substr);
extern char *util_strcat(const char *srcstr1, const char *srcstr2);
extern int util_file_is_exist(const char *pathname);
extern char *util_dir_append(const char *srcdir, const char *newdir);
extern const char *util_get_file_title(const char *strpath);

extern unsigned int util_get_m_second();
extern int execute_script(const char *command);

extern void trim_left_space(char *srcstr);
extern void output_byte_array(const unsigned char *buffer, int size, int max_output);
extern int four_bytes_to_int(unsigned char *data, int size);
extern unsigned int four_bytes_to_uint(unsigned char *data, int data_len);

extern int int_to_four_bytes(int value, unsigned char *buffer, int size);
extern void util_strncpy(char *dest, const char *srcstr, size_t size);
extern int util_create_dir(const char *pathname);
extern void reboot_device();
extern int get_host_ip_running(host_ip_callback callback);

#endif /* __UTIL_FUNC_H__ */
