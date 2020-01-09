
set(kwin3_kstep_PART_SRCS nextclient.cpp )


kde4_add_plugin(kwin3_kstep ${kwin3_kstep_PART_SRCS})



target_link_libraries(kwin3_kstep  ${KDE4_KDEUI_LIBS} kdecorations )

install(TARGETS kwin3_kstep  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES kstep.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )




#original Makefile.am contents follow:

#
#INCLUDES = $(all_includes) 
#
#kde_module_LTLIBRARIES = kwin3_kstep.la
#
#kwin3_kstep_la_SOURCES = nextclient.cpp
#kwin3_kstep_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
##kwin_kstep_la_LDFLAGS = $(all_libraries) -avoid-version -module $(KDE_RPATH) $(KDE_MT_LDFLAGS)
#kwin3_kstep_la_LIBADD = $(LIB_KDEUI) -lkdecorations
#
#METASOURCES = AUTO
#noinst_HEADERS = nextclient.h
#
#lnkdir = $(kde_datadir)/kwin/
#lnk_DATA = kstep.desktop
#
#EXTRA_DIST = $(lnk_DATA)
#
#
####KMAKE-start (don't edit or delete this block)
#
####KMAKE-end
