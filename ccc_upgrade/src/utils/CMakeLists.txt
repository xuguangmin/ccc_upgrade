configure_file(util_func.h       ${CUSTOM_INCLUDE_DIR_MAIN}/util_func.h)
configure_file(util_log.h        ${CUSTOM_INCLUDE_DIR_MAIN}/util_log.h)
configure_file(thread_wrapper.h  ${CUSTOM_INCLUDE_DIR_MAIN}/thread_wrapper.h)

aux_source_directory(. UTILS_SRC_LIST)
add_library(utils STATIC  ${UTILS_SRC_LIST})
