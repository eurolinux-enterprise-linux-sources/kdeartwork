



########### next target ###############

set(kwin3_kde1_PART_SRCS kde1client.cpp )

kde4_add_plugin(kwin3_kde1 ${kwin3_kde1_PART_SRCS})



target_link_libraries(kwin3_kde1  ${KDE4_KDECORE_LIBS} kdecorations )

install(TARGETS kwin3_kde1  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kde1.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )




#original Makefile.am contents follow:

#
#INCLUDES = $(all_includes) 
#
#kde_module_LTLIBRARIES = kwin3_kde1.la
#
#kwin3_kde1_la_SOURCES = kde1client.cpp
#kwin3_kde1_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#kwin3_kde1_la_LIBADD  = -lkdecorations
#
#METASOURCES = AUTO
#noinst_HEADERS = kde1client.h
#
#lnkdir = $(kde_datadir)/kwin/
#lnk_DATA = kde1.desktop
#EXTRA_DIST = $(lnk_DATA)
#
