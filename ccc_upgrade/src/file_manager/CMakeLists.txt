configure_file(file_manager.h  ${CUSTOM_INCLUDE_DIR_MAIN}/file_manager/file_manager.h)
add_library(file_manager STATIC file_manager.c upload_file.c md5sum.c )
