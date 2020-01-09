project(kdeartwork)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

# search packages used by KDE
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})


macro_optional_add_subdirectory(ColorSchemes)
macro_optional_add_subdirectory(IconThemes)
macro_optional_add_subdirectory(emoticons)
if (Q_WS_X11)
   macro_optional_add_subdirectory(kscreensaver)
   message(STATUS "Remember to readd kwin-style when ported")
   #macro_optional_add_subdirectory(kwin-styles)
endif (Q_WS_X11)

macro_optional_add_subdirectory(sounds)
macro_optional_add_subdirectory(styles)
macro_optional_add_subdirectory(wallpapers)
macro_optional_add_subdirectory(WeatherWallpapers)
macro_optional_add_subdirectory(desktopthemes)

macro_display_feature_log()

