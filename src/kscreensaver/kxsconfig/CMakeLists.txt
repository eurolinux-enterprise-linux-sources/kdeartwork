configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kxsconfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kxsconfig.h )

########### next target ###############

set(kxsconfig_SRCS 
   kxscontrol.cpp 
   kxsitem.cpp 
   kxsxml.cpp 
   kxsconfig.cpp )


kde4_add_executable(kxsconfig ${kxsconfig_SRCS})

target_link_libraries(kxsconfig ${KDE4_KIO_LIBS} ${X11_Xt_LIB})

install(TARGETS kxsconfig  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kxsrun_SRCS 
   kxsrun.cpp 
   kxscontrol.cpp 
   kxsitem.cpp 
   kxsxml.cpp )

kde4_add_executable(kxsrun ${kxsrun_SRCS})

target_link_libraries(kxsrun  ${KDE4_KIO_LIBS})

install(TARGETS kxsrun  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

FILE(GLOB _desktopfiles "ScreenSavers/*.desktop" )
foreach(_currentdesktopfile ${_desktopfiles})
	STRING(REGEX REPLACE ".desktop" "" _newitem "${_currentdesktopfile}" )
	#MESSAGE(STATUS "newItem <${_newitem}>")
	GET_FILENAME_COMPONENT(_screensaverName ${_newitem} NAME_WE)
	#MESSAGE(STATUS "name without extension <${_screensaverName}>")
	find_file(XSCREENSAVER_FILE_FOUND ${_screensaverName}.xml PATHS ${XSCREENSAVER_CONFIG_DIR} )
	if(XSCREENSAVER_FILE_FOUND)
		MESSAGE(STATUS "xscreensaver name ${_screensaverName} found")
		install(FILES ${_currentdesktopfile} DESTINATION  ${SERVICES_INSTALL_DIR}/ScreenSavers )
	else(XSCREENSAVER_FILE_FOUND)
		MESSAGE(STATUS "xscreensaver name ${_screensaverName} not found")
	endif(XSCREENSAVER_FILE_FOUND)
endforeach(_currentdesktopfile ${_desktopfiles})


#original Makefile.am contents follow:

### Makefile.am of kdebase/kscreensaver/kxsconfig
#
#INCLUDES = $(all_includes)
#kxsconfig_LDFLAGS  = $(KDE_RPATH) $(all_libraries) 
#kxsconfig_LDADD    = $(LIB_KIO) -lXt
#
#kxsrun_LDFLAGS  = $(KDE_RPATH) $(all_libraries) 
#kxsrun_LDADD    = $(LIB_KIO)
#
#EXTRA_DIST = ScreenSavers hacks.pot
#
######## Files
#
#bin_PROGRAMS = kxsconfig kxsrun
#
#kxsconfig_SOURCES = kxscontrol.cpp kxsitem.cpp kxsxml.cpp kxsconfig.cpp
#kxsrun_SOURCES = kxsrun.cpp kxscontrol.cpp kxsitem.cpp kxsxml.cpp
#
#noinst_HEADERS = kxsconfig.h kxscontrol.h kxsitem.h kxsxml.h
#
#METASOURCES = AUTO
#
######## Build rules
#
#messages:
#	$(XGETTEXT) -x hacks.pot *.cpp *.h -o $(podir)/kxsconfig.pot
#	cat hacks.pot >> $(podir)/kxsconfig.pot
#	-rm -f labels.cpp
#
#newhacks:
#	hacks=`cd /usr/lib/xscreensaver/ && ls -1 | grep -v config` ;\
#	echo $$hacks ;\
#	for name in $$hacks; do \
#		if test -f /usr/lib/xscreensaver/$$name && test -f /etc/xscreensaver/$$name.xml && test ! -f $(srcdir)/ScreenSavers/$$name.desktop; then \
#		label=`grep '<screensaver.*_label' /etc/xscreensaver/$$name.xml  | sed -e 's#^.*_label=\"\(.*\)\".*$$#\1#'` ;\
#		f=$(srcdir)/ScreenSavers/$$name.desktop ;\
#		sed -e "s#@NAME@#$$name#; s#@LABEL@#$$label#" $(srcdir)/ScreenSavers/xscreensaver.template > $$f; \
#		if ldd /usr/lib/xscreensaver/$$name 2>&1 | grep libGL; then sed -e "s,@GL1@,X-KDE-Type=OpenGL,; s,@GL2@,X-KDE-Category=OpenGL Screen Savers," $$f > $$f.new && mv $$f.new $$f; else grep -v @GL $$f > $$f.new && mv $$f.new $$f; fi ;\
#	    fi; \
#	done
#
#screensaverdir=$(kde_appsdir)/System/ScreenSavers
#
#install-data-local:
#	$(mkinstalldirs) $(DESTDIR)$(screensaverdir)
#	list=`(cd $(srcdir)/ScreenSavers && ls -1 *.desktop)` ;\
#	for i in $$list; do \
#		hack=`echo $$i | sed -e "s#\.desktop##"` ;\
#		if test -x $(XSCREENSAVER_HACKS_DIR)/$$hack; then \
#			echo "$$hack found" ;\
#			echo $(INSTALL_DATA) $(srcdir)/ScreenSavers/$$i $(DESTDIR)$(screensaverdir)/$$i ;\
#			$(INSTALL_DATA) $(srcdir)/ScreenSavers/$$i $(DESTDIR)$(screensaverdir)/$$i ;\
#		else \
#			echo "$$hack not found" ;\
#		fi ;\
#	done 
