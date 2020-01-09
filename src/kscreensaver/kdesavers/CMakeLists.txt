check_include_files(limits      HAVE_NUMERIC_LIMITS)
configure_file (config-kdesavers.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kdesavers.h )

macro_optional_find_package(Eigen2)
macro_log_feature(EIGEN2_FOUND "Eigen2" "A C++ template library for linear algebra" "kdesupport" FALSE "" "Needed to build the krotation and kpendulum screensavers" )

option(KSCREENSAVER_SOUND_SUPPORT "Build with sound support" OFF)

include_directories(${KDE4WORKSPACE_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

add_subdirectory( data )
macro_optional_add_subdirectory( asciiquarium )


########### next target ###############

set(kvm.kss_SRCS kvm.cpp vm.c vm_random.c )

kde4_add_executable(kvm.kss ${kvm.kss_SRCS})

target_link_libraries(kvm.kss ${KDE4_KDEUI_LIBS} ${KDE4WORKSPACE_KSCREENSAVER_LIBRARY})

install(TARGETS kvm.kss  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kclock.kss_SRCS kclock.cpp )

kde4_add_executable(kclock.kss ${kclock.kss_SRCS})

target_link_libraries(kclock.kss ${KDE4_KDEUI_LIBS} m ${KDE4WORKSPACE_KSCREENSAVER_LIBRARY})

install(TARGETS kclock.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )
install(FILES KClock.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ScreenSavers)


########### next target ###############

message(STATUS "readd opengl+sound support screensaver")
if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND KSCREENSAVER_SOUND_SUPPORT)
#set(kfiresaver.kss_SRCS firesaverparticle.cpp firesaverwriter.cpp firesaver.cpp )
#kde4_add_ui3_files(kfiresaver.kss_SRCS aversetup.ui)
##kde4_add_executable(kfiresaver.kss ${kfiresaver.kss_SRCS})
#target_link_libraries(kfiresaver.kss  ${KDE4_KDEUI_LIBS} )
#install(TARGETS kfiresaver.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )
#install( FILES KFiresaver.desktop DESTINATION  ${SERVICES_INSTALL_DIR}/ScreenSavers )
endif(OPENGL_FOUND AND OPENGL_GLU_FOUND AND KSCREENSAVER_SOUND_SUPPORT)

########### next target ###############
if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_LIBRARY)

  ## krotation, kpendulum need Eigen2
  if (EIGEN2_FOUND)
    include_directories(${EIGEN2_INCLUDE_DIR})

    set(krotation.kss_SRCS rotation.cpp sspreviewarea.cpp)
    kde4_add_ui_files(krotation.kss_SRCS rotationcfg.ui)
    kde4_add_executable(krotation.kss ${krotation.kss_SRCS})
    target_link_libraries(krotation.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${KDE4WORKSPACE_KSCREENSAVER_LIBRARY})
    install(TARGETS krotation.kss ${INSTALL_TARGETS_DEFAULT_ARGS})

    set(kpendulum.kss_SRCS pendulum.cpp sspreviewarea.cpp)
    kde4_add_ui_files(kpendulum.kss_SRCS pendulumcfg.ui)
    kde4_add_executable(kpendulum.kss ${kpendulum.kss_SRCS})
    target_link_libraries(kpendulum.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${KDE4WORKSPACE_KSCREENSAVER_LIBRARY})
    install(TARGETS kpendulum.kss ${INSTALL_TARGETS_DEFAULT_ARGS})
  endif (EIGEN2_FOUND)

  set(ksolarwinds.kss_SRCS SolarWinds.cpp )
  kde4_add_executable(ksolarwinds.kss ${ksolarwinds.kss_SRCS})
  target_link_libraries( ksolarwinds.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${KDE4WORKSPACE_KSCREENSAVER_LIBRARY})
  install(TARGETS ksolarwinds.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )

  set(kflux.kss_SRCS Flux.cpp )
  kde4_add_executable(kflux.kss ${kflux.kss_SRCS})
  target_link_libraries( kflux.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${KDE4WORKSPACE_KSCREENSAVER_LIBRARY})
  install(TARGETS kflux.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )

  set(keuphoria.kss_SRCS Euphoria.cpp )
  kde4_add_executable(keuphoria.kss ${keuphoria.kss_SRCS})
  target_link_libraries( keuphoria.kss ${KDE4_KDEUI_LIBS} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${KDE4WORKSPACE_KSCREENSAVER_LIBRARY})
  install(TARGETS keuphoria.kss ${INSTALL_TARGETS_DEFAULT_ARGS} )

  install( FILES KFlux.desktop KEuphoria.desktop KSolarWinds.desktop KRotation.desktop KPendulum.desktop DESTINATION  ${SERVICES_INSTALL_DIR}/ScreenSavers )

endif(OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_LIBRARY)

########### install files ###############

install( FILES kscience.png particle.png image.png  DESTINATION  ${DATA_INSTALL_DIR}/kscreensaver )
install( FILES KVm.desktop DESTINATION  ${SERVICES_INSTALL_DIR}/ScreenSavers )

