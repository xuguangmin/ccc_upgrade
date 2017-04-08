configure_file(protocol_adapter.h      ${CUSTOM_INCLUDE_DIR_MAIN}/protocol_adapter/protocol_adapter.h)

add_library(protocol_adapter STATIC  protocol_adapter.c)
