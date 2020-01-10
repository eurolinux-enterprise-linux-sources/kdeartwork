add_subdirectory( config ) 

########### next target ###############

set(kwin3_modernsys_PART_SRCS modernsys.cpp)


kde4_add_plugin(kwin3_modernsys ${kwin3_modernsys_PART_SRCS})

target_link_libraries(kwin3_modernsys
   ${KDE4WORKSPACE_KDECORATIONS_LIBS}
   ${KDE4_KDEUI_LIBS}
   ${QT_QTGUI_LIBRARY}
   ${QT_QTCORE_LIBRARY})

install(TARGETS kwin3_modernsys  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES modernsystem.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )

