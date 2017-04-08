/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞利信科技股份有限公司

 ******************************************************************************
  文件名称 : configure.c
  作者    : 贾延刚
  生成日期 : 2012-10

  版本    : 1.0
  功能描述 : 封装了程序配置

  修改历史 :

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "util_func.h"
#include "configure.h"
#include "ccc_config.h"


static char *l_workdir_upgrade  = NULL;        /* 工作目录下的升级目录 */

/*
 * 获取升级目录
 * 需要free结果
 */
char *util_get_upgrade_dir()
{
	char *result = NULL;
	char *current_dir = getcwd(NULL, 0);

	result = util_dir_append(current_dir, CCC_UPGRADE_DIRECTORY);
	if(current_dir)free(current_dir);

	return result;
}

const char *get_upgrade_dir()
{
	return l_workdir_upgrade;
}


int check_login(const char *login_info)
{
	int result = 0;
	if(0 == strcmp(login_info, "jikongupgrade")) result = 1;

	//printf("%s password :%s\n", __FUNCTION__,  login_info);
	return result;
}

void close_configure()
{
	if(l_workdir_upgrade) free(l_workdir_upgrade);
	l_workdir_upgrade = NULL;
}

int load_configure()
{
	l_workdir_upgrade = util_get_upgrade_dir();
	if(!l_workdir_upgrade)
	{
		printf("%s upgrade_directory = NULL\n", __FUNCTION__);
		return 0;
	}
	return 1;
}
