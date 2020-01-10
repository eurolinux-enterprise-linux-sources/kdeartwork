find_package( KDE4Workspace REQUIRED )
include_directories( ${KDE4WORKSPACE_INCLUDE_DIR} )

add_subdirectory( kde2 )
add_subdirectory( keramik )
add_subdirectory( modernsystem )
add_subdirectory( quartz )
add_subdirectory( redmond )
add_subdirectory( web )
