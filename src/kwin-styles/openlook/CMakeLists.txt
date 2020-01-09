



########### next target ###############

set(kwin3_openlook_PART_SRCS OpenLook.cpp )

kde4_add_plugin(kwin3_openlook ${kwin3_openlook_PART_SRCS})



target_link_libraries(kwin3_openlook  ${KDE4_KDECORE_LIBS} kdecorations )

install(TARGETS kwin3_openlook  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES openlook.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )




#original Makefile.am contents follow:

#INCLUDES                    = $(all_includes)
#
#kde_module_LTLIBRARIES      = kwin3_openlook.la
#
#kwin3_openlook_la_SOURCES   = OpenLook.cpp
#kwin3_openlook_la_LDFLAGS   = $(all_libraries) $(KDE_PLUGIN) -module
#kwin3_openlook_la_LIBADD    = -lkdecorations
#
#noinst_HEADERS              = OpenLook.h
#METASOURCES                 = AUTO
#
#linkdir                     = $(kde_datadir)/kwin/
#link_DATA                   = openlook.desktop
#
#EXTRA_DIST                  = $(link_DATA)
#
