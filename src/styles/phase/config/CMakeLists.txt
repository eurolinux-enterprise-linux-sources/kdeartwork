
########### next target ###############

set(kstyle_phase_config_PART_SRCS phasestyleconfig.cpp )


kde4_add_ui_files(kstyle_phase_config_PART_SRCS styledialog.ui )

kde4_add_plugin(kstyle_phase_config ${kstyle_phase_config_PART_SRCS})

target_link_libraries(kstyle_phase_config  ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY})

install(TARGETS kstyle_phase_config  DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

