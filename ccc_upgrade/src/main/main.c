/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞利信科技股份有限公司

 ******************************************************************************
  文件名称 : main.c
  作者    :
  生成日期 :

  版本    : 1.0
  功能描述 :升级程序的服务端
           接收客户端上传文件的请求；接受查询本机IP地址的UDP请求

  /home/flx/developer/workspace_c/ccc_upgrade/build

******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <execinfo.h>
#include <signal.h>
#include <string.h>
#include "ccc_config.h"
#include "util_log.h"
#include "ccc_upgrade.h"
#include "configure.h"


static void print_trace(void)
{
	int size;
	char **symbols;
	void *array[1024];

	size = backtrace(array, 1024);
	CCC_LOG_OUT("segmentation fault\n backtrace stack deep %d\n", size);

	symbols = backtrace_symbols(array, size);
	if(symbols)
	{
		int k;
		for(k = 0; k < size; ++k)
		{
			CCC_LOG_OUT("%d: %s\n", k, symbols[k]);
		}
		free(symbols);
	}
	CCC_LOG_OUT("\n");
	exit(-1);
}

static void debug_backtrace(void)
{
	print_trace();
}

static void close_release_res(void)
{
	// kill(getpid(), SIGINT);
	exit(0);
}

int main(int argc, char *argv[])
{
	signal(SIGSEGV, (void *)debug_backtrace);    /* catch Segmentation fault signal */
	signal(SIGINT,  (void *)close_release_res);  /* reg ctrl+c signal */

#ifdef CCC_UPGRADE_DEBUG
	CCC_LOG_OUT("dubug version :%s ...\n", __FUNCTION__);
#else
	if(daemon(1, 0) != 0)
	{
		CCC_LOG_OUT("error daemon");
		return -1;
	}
#endif

	if(!load_configure())
	{
		CCC_LOG_OUT("%s load_configure error!\n", __FUNCTION__);
		return -1;
	}

	if(!start_ccc(CCC_UPGRADE_PORT_SERVER, get_upgrade_dir()))
	{
		CCC_LOG_OUT("%s start_ccc error!\n", __FUNCTION__);
		return -1;
	}
	return 0;
}
