configure_file(protocol_define.h       ${CUSTOM_INCLUDE_DIR_MAIN}/protocol/protocol_define.h)
configure_file(protocol_core.h         ${CUSTOM_INCLUDE_DIR_MAIN}/protocol/protocol_core.h)
configure_file(protocol.h              ${CUSTOM_INCLUDE_DIR_MAIN}/protocol/protocol.h)

add_library(protocol STATIC  protocol_core.c)
