#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#include "util_log.h"
#include "util_func.h"
#include "upload_file.h"
#include "md5sum.h"
#include "file_manager.h"
#include "protocol/protocol_define.h"


static FILE *open_tx_filename(const char *filename)
{
	FILE *pfile = fopen(filename, "w+");
	if (!pfile)
	{
		CCC_LOG_OUT("create file %s error : %s\n", filename, strerror(errno));
	}
	else{
		CCC_LOG_OUT("create file success : %s\n", filename);
	}
	return pfile;
}


static int check_file_by_md5(const char *check_filename, const char *md5sum)
{
	char sum_buffer[256];

	bzero(sum_buffer, 256);
	if(!calcu_md5_checksum(check_filename, sum_buffer, 255))
		return 0;

	if(strncasecmp(md5sum, sum_buffer, strlen(sum_buffer)) != 0)
	{
		CCC_LOG_OUT("MD5 :%s(client) != %s(server)\n", md5sum, sum_buffer);
		return 0;
	}
	return 1;
}

/*
 * 单个文件上传完
 * 参数：
 *     md5sum            MD5字符串
 *
 */
int file_upload_single_finish(const char *md5sum)
{
	int result = 0;
	const char *file_path;
	FILE *handle = upload_file_get_handle();
	if(handle) fclose(handle);

	file_path = upload_file_get_path();
	result = check_file_by_md5(file_path, md5sum);

	upload_file_clear();
	return result;
}

int file_upload_write(long int offset, const uint8_t *data_buf, int data_len)
{
	FILE *handle = upload_file_get_handle();
	if(handle)
	{
		if(0 == fseek(handle, offset, SEEK_SET))
		{
			if(fwrite(data_buf, sizeof(uint8_t), data_len, handle) == data_len)
				return 1;
		}
	}
	return 0;
}

static int check_upgrade_directory(const char *temp_dir)
{
	if(!temp_dir)
		return 0;

	if(util_file_is_exist(temp_dir))
		return 1;

	if(!util_create_dir(temp_dir))
	{
		printf("%s :create directory %s failed.\n", __FUNCTION__, temp_dir);
		return 0;
	}
	return 1;
}

/*
 * 在指定目录创建一个文件，保存上传文件内容
 *
 * 参数：
 *     dir           文件在该目录打开
 *     filetitle     文件名，不包含路径
 */
int file_upload_open(const char *dir, const char *filetitle)
{
	int result;
	FILE *file_handle;
	char *filename_tx;
	if(!dir || strlen(dir) <= 0)
	    return 0;
    if(!filetitle || strlen(filetitle) <= 0)
    	return 0;

    if(!check_upgrade_directory(dir))
    {
    	CCC_LOG_OUT("%s check_upgrade_directory failed.\n", __FUNCTION__);
    	return 0;
    }
    filename_tx = util_dir_append(dir, filetitle);
    if(!filename_tx)
    {
    	CCC_LOG_OUT("filename_tx = NULL");
		return 0;
    }

    file_handle = open_tx_filename(filename_tx);
    if(!file_handle)
	{
		free(filename_tx);
		return 0;
	}

    /* save to link list */
    result = upload_file_save(filetitle, filename_tx, file_handle);
	return result;
}

/*
 * 参数：
 *     dir           文件所在目录
 *     filetitle     文件名，不包含路径
 * 返回值：
 *      0 执行成功
 *      1 文件不存在
 *      2 未知错误
 *      3 修改权限不成功
 */
int file_execute_script(const char *dir, const char *filetitle)
{
	char *filename;
	if(!dir || strlen(dir) <= 0)
		return 0;
	if(!filetitle || strlen(filetitle) <= 0)
		return 0;

	filename = util_dir_append(dir, filetitle);
	if(!filename)
	{
		CCC_LOG_OUT("error :file_execute_script");
		return 2;
	}

	if(!util_file_is_exist(filename))
	{
		CCC_LOG_OUT("%s util_file_is_exist :%s", __FUNCTION__, strerror(errno));
		free(filename);
		return 1;
	}

	if(chmod(filename, S_IRUSR|S_IWUSR|S_IXUSR) != 0)
	{
		CCC_LOG_OUT("%s chmod :%s", __FUNCTION__, strerror(errno));
		free(filename);
		return 3;
	}

	execute_script(filename);
	free(filename);
	return 0;
}
