configure_file(ccc_config.h        ${CUSTOM_INCLUDE_DIR_MAIN}/ccc_config.h)

add_executable(ccc_upgrade main.c ccc_upgrade.c)
target_link_libraries(ccc_upgrade  libprotocol_adapter.a; libprotocol.a; libnetwork.a; libpacket_pool.a; libfile_manager.a; libconfigure.a; libutils.a; -ldl; -lpthread)


# ------------ install ------------
install(TARGETS ccc_upgrade RUNTIME DESTINATION release)
