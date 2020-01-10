project(kdeartwork)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

# search packages used by KDE
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})


macro_optional_add_subdirectory(ColorSchemes)
macro_optional_add_subdirectory(IconThemes)
macro_optional_add_subdirectory(emoticons)
macro_optional_add_subdirectory(kscreensaver)
if (Q_WS_X11)
   macro_optional_add_subdirectory(kwin-styles)
endif (Q_WS_X11)
macro_optional_add_subdirectory(sounds)
macro_optional_add_subdirectory(styles)
macro_optional_add_subdirectory(wallpapers)
macro_optional_add_subdirectory(HighResolutionWallpapers)
macro_optional_add_subdirectory(WeatherWallpapers)
macro_optional_add_subdirectory(desktopthemes)

macro_display_feature_log()

