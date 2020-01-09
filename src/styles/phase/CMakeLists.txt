
project(kstyle-phasestyle)

add_definitions(-DQT_PLUGIN)

add_subdirectory( config ) 

########### next target ###############

set(phasestyle_PART_SRCS phasestyle.cpp)

kde4_add_plugin(phasestyle ${phasestyle_PART_SRCS} )

target_link_libraries(phasestyle ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})

install(TARGETS phasestyle DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles/ )

########### install files ###############

install( FILES phase.themerc DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes )

