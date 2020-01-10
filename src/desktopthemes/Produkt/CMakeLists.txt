set(THEME_NAME "Produkt")

install(FILES colors metadata.desktop DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/)

FILE(GLOB widgets widgets/*.svgz)
install( FILES ${widgets} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/widgets/ )

FILE(GLOB dialogs dialogs/*.svgz)
install( FILES ${dialogs} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/dialogs/ )

FILE(GLOB opaque_dialogs opaque/dialogs/*.svgz)
install( FILES ${opaque_dialogs} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/opaque/dialogs/ )

FILE(GLOB opaque_widgets opaque/widgets/*.svgz)
install( FILES ${opaque_widgets} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/opaque/widgets/ )

FILE(GLOB translucent_widgets translucent/widgets/*.svgz)
install( FILES ${translucent_widgets} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/translucent/widgets/ )

FILE(GLOB translucent_dialogs translucent/dialogs/*.svgz)
install( FILES ${translucent_dialogs} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/translucent/dialogs/ )

FILE(GLOB icons icons/*.svgz)
install( FILES ${icons} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/${THEME_NAME}/icons/ )
