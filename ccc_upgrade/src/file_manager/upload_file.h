/******************************************************************************

                  ��Ȩ���� (C), 2001-2020, ���������ſƼ��ɷ����޹�˾

 ******************************************************************************
  �� �� ��   : upload_file.h
  ��   ��   : ���Ӹ�
  ��������   : 2012-10-26

  ��������   :
             ���浱ǰ�����ϴ����ļ���һЩ��Ϣ

******************************************************************************/

#ifndef __UPLOAD_FILE_H__
#define __UPLOAD_FILE_H__

#include <stdio.h>

extern int upload_file_save(const char *filetitle, char *filename_tx, FILE *handle);
extern int upload_file_clear();

extern FILE *upload_file_get_handle();
extern const char *upload_file_get_path();


#endif /* __UPLOAD_FILE_H__ */
