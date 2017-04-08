#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

#include <stdint.h>

extern int file_upload_open(const char *dir, const char *filetitle);
extern int file_upload_write(long int offset, const uint8_t *data_buf, int data_len);
extern int file_upload_single_finish(const char *md5sum);

extern int file_execute_script(const char *dir, const char *filetitle);

#endif  /* __FILE_MANAGER_H__ */
