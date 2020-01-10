set(kwin3_redmond_PART_SRCS redmond.cpp)

kde4_add_plugin(kwin3_redmond ${kwin3_redmond_PART_SRCS})

target_link_libraries(kwin3_redmond
   ${KDE4WORKSPACE_KDECORATIONS_LIBS}
   ${KDE4_KDEUI_LIBS}
   ${QT_QTGUI_LIBRARY}
   ${QT_QTCORE_LIBRARY})

install(TARGETS kwin3_redmond  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES redmond.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin )
