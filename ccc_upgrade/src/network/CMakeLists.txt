configure_file(network_share.h       ${CUSTOM_INCLUDE_DIR_MAIN}/network/network_share.h)
configure_file(network_manager.h     ${CUSTOM_INCLUDE_DIR_MAIN}/network/network_manager.h)
configure_file(udp_server.h          ${CUSTOM_INCLUDE_DIR_MAIN}/network/udp_server.h)

aux_source_directory(. NETWORK_SRC_LIST)
add_library(network STATIC  ${NETWORK_SRC_LIST})
