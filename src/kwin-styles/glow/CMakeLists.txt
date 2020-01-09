
add_subdirectory( config ) 
add_subdirectory( themes ) 




########### next target ###############

set(kwin3_glow_PART_SRCS glowclient.cpp glowbutton.cpp )


kde4_add_plugin(kwin3_glow ${kwin3_glow_PART_SRCS})



target_link_libraries(kwin3_glow  ${KDE4_KDECORE_LIBS} kdecorations )

install(TARGETS kwin3_glow  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES glow.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )




#original Makefile.am contents follow:

#
#INCLUDES = $(all_includes)
#
#SUBDIRS = . config themes
#
#kde_module_LTLIBRARIES = kwin3_glow.la
#
#kwin3_glow_la_SOURCES = glowclient.cpp glowbutton.cpp
#kwin3_glow_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#kwin3_glow_la_LIBADD  = $(LIB_KDECORE) -lkdecorations
#
#METASOURCES = AUTO
#noinst_HEADERS = glowclient.h glowbutton.h resources.h
#
#linkdir = $(kde_datadir)/kwin/
#link_DATA = glow.desktop
#
#EXTRA_DIST = $(link_DATA)
#
####KMAKE-start (don't edit or delete this block)
#
####KMAKE-end
