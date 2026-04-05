configure_file(qtchooser_config.h.in ${PROJECT_BINARY_DIR}/qtchooser/qtchooser_config.h)

add_subdirectory(qtchooserlib)
add_subdirectory(cli)
