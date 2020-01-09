



########### next target ###############

set(kwin_icewm_config_PART_SRCS config.cpp )

kde4_add_plugin(kwin_icewm_config ${kwin_icewm_config_PART_SRCS})



target_link_libraries(kwin_icewm_config    ${KDE4_KIO_LIBS} )

install(TARGETS kwin_icewm_config  DESTINATION ${PLUGIN_INSTALL_DIR} )



#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#
#kde_module_LTLIBRARIES = kwin_icewm_config.la
#
#kwin_icewm_config_la_SOURCES = config.cpp
#kwin_icewm_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#kwin_icewm_config_la_LIBADD = $(LIB_KDEUI) $(LIB_KIO) 
#
#METASOURCES = AUTO
#noinst_HEADERS = config.h
#
#lnkdir = $(kde_datadir)/kwin/
#
####KMAKE-start (don't edit or delete this block)
#
####KMAKE-end
