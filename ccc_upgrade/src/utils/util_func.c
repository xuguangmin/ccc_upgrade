/*
 * util_func.c
 *
 *  Created on: 2012-11-20
 *      Author: flx
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "util_func.h"



#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>


/*
 * 复制srcstr，
 * 内部会分配内存，返回值的内存需要使用者释放
 */
char *util_strcpy(const char *srcstr)
{
	char *result = NULL;
	if(srcstr)
	{
		int len = strlen(srcstr);
		if(len > 0)
		{
			result = (char *)malloc(len + 1);
			if(result)
			{
				strcpy(result, srcstr);
				result[len] = '\0';
			}
		}
	}
	return result;
}

void util_strncpy(char *dest, const char *srcstr, size_t size)
{
	if(dest && srcstr && size > 0)
	{
		int len = strlen(srcstr);
		if(len > 0)
		{
			len = (len > size) ? size:len;
			strncpy(dest, srcstr, len);
			dest[len]='\0';
		}
	}
}

const char *util_strcasestr(const char *srcstr, const char *substr)
{
	int len;
	if(!srcstr || !substr)
		return NULL;

    len = strlen(substr);
    if(len <= 0 || strlen(srcstr) <= 0)
        return NULL;                                   /* 这里我并未照strstr一样返回str，而是返回NULL */

    while(*srcstr)
    {
        if(strncasecmp(srcstr, substr, len) == 0)       /* 这里使用了可限定比较长度的strncasecmp */
            return srcstr;

        srcstr++;
    }
    return NULL;
}

/*
 * 从路径中取文件名，返回起始位置的指针
 * 如果没有/，则直接返回原字符串
 */
const char *util_get_file_title(const char *strpath)
{
	char *p;
	if(!strpath || strlen(strpath) <= 0)
		return NULL;

	p = rindex(strpath, '/');
	if(!p) return strpath;

	p++;
	return p;
}

/* 得到毫秒时间 */
unsigned int util_get_m_second()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec/1000);
}

void trim_left_space(char *srcstr)
{
	char *p;
	if(!srcstr || strlen(srcstr) <= 0)
		return;

	p = srcstr;
	while(*p)
	{
		if(*p != 0x20) break;
		p++;
	}

	if(p != srcstr) strcpy(srcstr, p);
}

/*
 * 参数：
 *
 *      max_output  最多输出多少数据
 */
void output_byte_array(const unsigned char *buffer, int size, int max_output)
{
	const unsigned char *p = buffer;
	int out_len = (size > max_output) ? max_output:size;
	while(out_len > 0)
	{
		printf("%02X", *p);
		out_len--;
		p++;
	}
	printf("\n");
}

unsigned int four_bytes_to_uint(unsigned char *data, int data_len)
{
	if(!data || data_len < 4)
		return 0;

	return ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
}

int four_bytes_to_int(unsigned char *data, int size)
{
	if(!data || size < 4)
		return 0;

	return ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
}
int int_to_four_bytes(int value, unsigned char *buffer, int size)
{
	if(!buffer || size < 4)
		return 0;

	buffer[0] = (value >> 24) & 0xFF;
	buffer[1] = (value >> 16) & 0xFF;
	buffer[2] = (value >> 8) & 0xFF;
	buffer[3] =  value & 0xFF;
	return 1;
}

int util_file_is_exist(const char *pathname)
{
	return (0 == access(pathname, F_OK)) ? 1:0;
}
int util_create_dir(const char *pathname)
{
	int k, len;
	char *dirname;
	if(!pathname)
		return 0;

	len = strlen(pathname);
	if(len <= 0)
		return 0;

	dirname = (char *)malloc(len + 2);
	if(!dirname)
		return 0;

	strcpy(dirname, pathname);
	dirname[len] = '\0';
	dirname[len+1] = '\0';


	if (dirname[len-1] != '/')
		strcat(dirname, "/");

	len = strlen(dirname);
	for (k = 1; k < len; k++)
	{
		if (dirname[k] == '/')
		{
			dirname[k] = 0;
			if (access(dirname, F_OK) != 0)
			{
				if (mkdir(dirname, 0755) == -1)
				{
					perror("mkdir   error");
					free(dirname);
					return 0;
				}
			}
			dirname[k] = '/';
		}
	}
	free(dirname);
	return 1;
}

/* 连接两个字符串
 * 结果为新的字符串，
 * 需要free结果
 */
char *util_strcat(const char *srcstr1, const char *srcstr2)
{
	char *result = NULL;
	int len1, len2, total;
	if(!srcstr1 || !srcstr2)
		return NULL;

	len1 = strlen(srcstr1);
	len2 = strlen(srcstr2);
	if(len1 <= 0 || len2 <= 0)
		return NULL;

	total = len1 + len2 + 1;
	result = (char *)malloc(total);
	if(!result)
		return NULL;

	memset(result, 0, total);
	strcpy(result, srcstr1);
	strcat(result, srcstr2);
	return result;
}

/*
 * 在一个目录上追加一个目录或者文件名
 * 最后没有 "/"
 * 需要free结果
 *
 */
char *util_dir_append(const char *srcdir, const char *newdir)
{
	char *result = NULL;
	int len1, len2, total;
	if(!srcdir || !newdir)
		return NULL;

	len1 = strlen(srcdir);
	len2 = strlen(newdir);
	if(len1 <= 0 || len2 <= 0)
		return NULL;

	total = len1 + 1 + len2 + 1;
	result = (char *)malloc(total);
	if(result)
	{
		memset(result, 0, total);
		strcpy(result, srcdir);
		len1 = strlen(result);
		if(result[len1-1] != '/')
			strcat(result, "/");

		strcat(result, newdir);
	}

	return result;
}




int get_host_ip_running(host_ip_callback callback)
{
	struct sockaddr_in *sin = NULL;
	struct ifaddrs *ifa = NULL, *ifList;

	if (!callback || getifaddrs(&ifList) < 0)
		return 0;

	for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr->sa_family == AF_INET)
		{
			if(0 == strcmp("lo", ifa->ifa_name))
				continue;

			if(ifa->ifa_flags & IFF_RUNNING)
			{
				char *ipaddr;
				uint8_t info[256];
				bzero(info, 256);

				sin = (struct sockaddr_in *)ifa->ifa_addr;
				ipaddr = inet_ntoa(sin->sin_addr);
				memcpy(info, ipaddr, strlen(ipaddr));

				callback(info, strlen((char *)info));
				//send_reply_to_udp(handle, PCS_DISCOVER_DEVICE, PCS_EX_01, info, strlen((char *)info));
			}
		}
	}
	freeifaddrs(ifList);
	return 1;
}

int execute_script(const char *command)
{
	printf("%s :%s\n", __FUNCTION__, command);
	system(command);
	return 1;
}

void reboot_device()
{
	system("/sbin/reboot");
}
