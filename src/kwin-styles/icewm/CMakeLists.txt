
add_subdirectory( config ) 
add_subdirectory( icewm-themes ) 




########### next target ###############

set(kwin3_icewm_PART_SRCS icewm.cpp )


kde4_add_plugin(kwin3_icewm ${kwin3_icewm_PART_SRCS})



target_link_libraries(kwin3_icewm   ${KDE4_KDEUI_LIBS} kdecorations )

install(TARGETS kwin3_icewm  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES icewm.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )




#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#
#SUBDIRS = . config icewm-themes
#
#kde_module_LTLIBRARIES = kwin3_icewm.la
#
#kwin3_icewm_la_SOURCES = icewm.cpp
#kwin3_icewm_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#kwin3_icewm_la_LIBADD  = $(LIB_KDEUI) -lkdecorations
#
#METASOURCES = AUTO
#noinst_HEADERS = icewm.h
#
#lnkdir = $(kde_datadir)/kwin/
#lnk_DATA = icewm.desktop
#
#EXTRA_DIST = $(lnk_DATA)
#
####KMAKE-start (don't edit or delete this block)
#
####KMAKE-end
