configure_file(packet_pool.h        ${CUSTOM_INCLUDE_DIR_MAIN}/packet_pool/packet_pool.h)
configure_file(ccc_packet.h         ${CUSTOM_INCLUDE_DIR_MAIN}/packet_pool/ccc_packet.h)
aux_source_directory(. PACKET_POOL_SRC_LIST)
add_library(packet_pool  ${PACKET_POOL_SRC_LIST})
