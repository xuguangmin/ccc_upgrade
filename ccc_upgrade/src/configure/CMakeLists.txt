configure_file(configure.h        ${CUSTOM_INCLUDE_DIR_MAIN}/configure.h)
aux_source_directory(. CONFIG_SRC_LIST)
add_library(configure STATIC  ${CONFIG_SRC_LIST})
