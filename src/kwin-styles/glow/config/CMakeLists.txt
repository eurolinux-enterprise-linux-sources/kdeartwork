



########### next target ###############

set(kwin_glow_config_PART_SRCS glowconfigdialog.cpp )

kde4_add_plugin(kwin_glow_config ${kwin_glow_config_PART_SRCS})



target_link_libraries(kwin_glow_config  ${KDE4_KDEUI_LIBS} )

install(TARGETS kwin_glow_config  DESTINATION ${PLUGIN_INSTALL_DIR} )





#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#
#kde_module_LTLIBRARIES = kwin_glow_config.la
#
#kwin_glow_config_la_SOURCES = glowconfigdialog.cpp
#kwin_glow_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#kwin_glow_config_la_LIBADD = $(LIB_KDEUI)
#
#METASOURCES = AUTO
#noinst_HEADERS = glowconfigdialog.h
#
#lnkdir = $(kde_datadir)/kwin/
#
#
####KMAKE-start (don't edit or delete this block)
#
####KMAKE-end
