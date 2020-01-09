
add_subdirectory( config ) 




########### next target ###############

set(kwin3_cde_PART_SRCS cdeclient.cpp )

kde4_add_plugin(kwin3_cde ${kwin3_cde_PART_SRCS})



target_link_libraries(kwin3_cde  ${KDE4_KDECORE_LIBS} kdecorations )

install(TARGETS kwin3_cde  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES cde.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )




#original Makefile.am contents follow:

#
#INCLUDES = $(all_includes) 
#
#SUBDIRS = . config
#
#kde_module_LTLIBRARIES = kwin3_cde.la
#
#kwin3_cde_la_SOURCES = cdeclient.cpp
#kwin3_cde_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#kwin3_cde_la_LIBADD = -lkdecorations
#
#METASOURCES = AUTO
#noinst_HEADERS = cdeclient.h
#
#lnkdir = $(kde_datadir)/kwin/
#lnk_DATA = cde.desktop
#
#EXTRA_DIST = $(lnk_DATA)
#
####KMAKE-start (don't edit or delete this block)
#
####KMAKE-end
