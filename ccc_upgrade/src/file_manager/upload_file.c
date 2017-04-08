/******************************************************************************

                  ��Ȩ���� (C), 2001-2020, ���������ſƼ��ɷ����޹�˾

 ******************************************************************************
  �� �� ��   : upload_file.c
  ��   ��   : ���Ӹ�
  ��������   : 2012-10-26

  ��������   :
             ���浱ǰ�����ϴ����ļ���һЩ��Ϣ

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct __UPLOAD_FILE_T
{
	char             *filename;                /* �ͻ����ύ���ļ��� */
	char             *filename_tx;             /* ����ʱʹ�õ��ļ��� */
	FILE             *file_handle;			   /* �򿪵��ļ���� */
}UPLOAD_FILE_T;
static UPLOAD_FILE_T   l_upload_current = {NULL, NULL, NULL};



static int internal_upload_file_clear(UPLOAD_FILE_T *lp_upload_file)
{
	free(lp_upload_file->filename);
	free(lp_upload_file->filename_tx);
	lp_upload_file->file_handle = NULL;
	return 1;
}

FILE *internal_upload_file_get_handle(UPLOAD_FILE_T *lp_upload_file)
{
	if(!lp_upload_file)
		return NULL;

	return lp_upload_file->file_handle;
}

static int internal_upload_file_save(UPLOAD_FILE_T *lp_upload_file, const char *filetitle,
		               char *filename_tx, FILE *handle)
{
	if(!lp_upload_file || !filetitle || !filename_tx || !handle)
		return 0;

	lp_upload_file->filename = strdup(filetitle);
	lp_upload_file->filename_tx = filename_tx;
	lp_upload_file->file_handle = handle;
	return 1;
}
static const char *internal_upload_file_get_path(UPLOAD_FILE_T *lp_upload_file)
{
	if(!lp_upload_file)
		return NULL;

	return lp_upload_file->filename_tx;

}

const char *upload_file_get_path()
{
	return internal_upload_file_get_path(&l_upload_current);
}

FILE *upload_file_get_handle()
{
	return internal_upload_file_get_handle(&l_upload_current);
}

/* ����ϴ��ļ�����Ϣ */
int upload_file_clear()
{
	return internal_upload_file_clear(&l_upload_current);
}
/* �����ϴ��ļ�����Ϣ */
int upload_file_save(const char *filetitle, char *filename_tx, FILE *handle)
{
	return internal_upload_file_save(&l_upload_current, filetitle, filename_tx, handle);
}
