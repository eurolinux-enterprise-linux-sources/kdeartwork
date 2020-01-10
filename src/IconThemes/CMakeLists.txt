
set(iconthemes nuvola mono)

set(typeIcons actions animations apps categories devices emblems emotes intl mimetypes places status)

set(sizeIcon 8x8 11x11 16x16 22x22 24x24 32x32 48x48 64x64 128x128 256x256 scalable)

set(kdedir kde)

foreach (_currentthemedir ${iconthemes})
   	install(FILES ${_currentthemedir}/index.theme DESTINATION ${ICON_INSTALL_DIR}/${_currentthemedir} )
   
	foreach(_currentsize ${sizeIcon})
		
		foreach(_currenttypeicon ${typeIcons})
			FILE(GLOB _pngfile "${_currentthemedir}/${_currentsize}/${_currenttypeicon}/*.png" )
			install(FILES ${_pngfile} DESTINATION ${ICON_INSTALL_DIR}/${_currentthemedir}/${_currentsize}/${_currenttypeicon})
			FILE(GLOB _mngfile "${_currentthemedir}/${_currentsize}/${_currenttypeicon}/*.mng")
			install(FILES ${_mngfile} DESTINATION ${ICON_INSTALL_DIR}/${_currentthemedir}/${_currentsize}/${_currenttypeicon})
			FILE(GLOB _svgfile "${_currentthemedir}/${_currentsize}/${_currenttypeicon}/*.svg")
			install(FILES ${_svgfile} DESTINATION ${ICON_INSTALL_DIR}/${_currentthemedir}/${_currentsize}/${_currenttypeicon})
			FILE(GLOB _svgzfile "${_currentthemedir}/${_currentsize}/${_currenttypeicon}/*.svgz")
			install(FILES ${_svgzfile} DESTINATION ${ICON_INSTALL_DIR}/${_currentthemedir}/${_currentsize}/${_currenttypeicon})
			# TODO fix it
			#foreach(_currentkdedir ${kdedir})
			#	FILE(GLOB _kdeicons "*.png
			#endforeach(_currentkdedir ${kdedir})		
		endforeach(_currenttypeicon ${typeIcons})

   	endforeach(_currentsize ${sizeIcon})

endforeach(_currentthemedir ${iconthemes})

