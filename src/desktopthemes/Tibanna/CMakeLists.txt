set(THEME_NAME "Tibanna")

install(FILES metadata.desktop DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/)

FILE(GLOB widgets widgets/*.svgz)
install( FILES ${widgets} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/widgets/ )

FILE(GLOB dialogs dialogs/*.svgz)
install( FILES ${dialogs} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/dialogs/ )

FILE(GLOB opaque opaque/widgets/*.svgz)
install( FILES ${opaque} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/opaque/widgets/ )

FILE(GLOB opaque opaque/dialogs/*.svgz)
install( FILES ${opaque} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/opaque/dialogs/ )

