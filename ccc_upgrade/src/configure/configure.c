/******************************************************************************

                  ��Ȩ���� (C), 2001-2020, ���������ſƼ��ɷ����޹�˾

 ******************************************************************************
  �ļ����� : configure.c
  ����    : ���Ӹ�
  �������� : 2012-10

  �汾    : 1.0
  �������� : ��װ�˳�������

  �޸���ʷ :

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


static char *l_workdir_upgrade  = NULL;        /* ����Ŀ¼�µ�����Ŀ¼ */

/*
 * ��ȡ����Ŀ¼
 * ��Ҫfree���
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
