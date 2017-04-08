/******************************************************************************

                  版权所有 (C), 2001-2020, 北京飞立信科技股份有限公司

 ******************************************************************************
  文 件 名   : upload_file.h
  作   者   : 贾延刚
  生成日期   : 2012-10-26

  功能描述   :
             保存当前正在上传的文件的一些信息

******************************************************************************/

#ifndef __UPLOAD_FILE_H__
#define __UPLOAD_FILE_H__

#include <stdio.h>

extern int upload_file_save(const char *filetitle, char *filename_tx, FILE *handle);
extern int upload_file_clear();

extern FILE *upload_file_get_handle();
extern const char *upload_file_get_path();


#endif /* __UPLOAD_FILE_H__ */
