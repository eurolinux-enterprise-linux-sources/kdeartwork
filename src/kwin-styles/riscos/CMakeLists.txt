



########### next target ###############

set(kwin3_riscos_PART_SRCS 
   AboveButton.cpp 
   Button.cpp 
   CloseButton.cpp 
   HelpButton.cpp 
   IconifyButton.cpp 
   LowerButton.cpp 
   Manager.cpp 
   MaximiseButton.cpp 
   Static.cpp 
   StickyButton.cpp )

kde4_add_plugin(kwin3_riscos ${kwin3_riscos_PART_SRCS})



target_link_libraries(kwin3_riscos  ${KDE4_KDEUI_LIBS} kdecorations )

install(TARGETS kwin3_riscos  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES riscos.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )




#original Makefile.am contents follow:

#AUTOMAKE_OPTIONS = foreign
#
#KDE_CXXFLAGS = -DQT_PLUGIN
#
#INCLUDES = $(all_includes) 
#
#kwindir = $(kde_datadir)/kwin/
#kwin_DATA = riscos.desktop
#
#noinst_HEADERS = AboveButton.h \
#                 Button.h \
#                 CloseButton.h \
#                 HelpButton.h \
#                 IconifyButton.h \
#                 LowerButton.h \
#                 Manager.h \
#                 MaximiseButton.h \
#                 Palette.h \
#                 Static.h \
#                 StickyButton.h
#
#kde_module_LTLIBRARIES = kwin3_riscos.la
#kwin3_riscos_la_SOURCES = AboveButton.cpp \
#                          Button.cpp \
#                          CloseButton.cpp \
#                          HelpButton.cpp \
#                          IconifyButton.cpp \
#                          LowerButton.cpp \
#                          Manager.cpp \
#                          MaximiseButton.cpp \
#                          Static.cpp \
#                          StickyButton.cpp
#
#kwin3_riscos_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
#kwin3_riscos_la_LIBADD = $(LIB_KDEUI) -lkdecorations
#kwin3_riscos_la_METASOURCES = AUTO
#
#EXTRA_DIST = $(kwin_DATA)
#
