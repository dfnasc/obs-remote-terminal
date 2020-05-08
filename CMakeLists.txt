project(remote-control-ui)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(APPLE)
	find_library(COCOA Cocoa)
	include_directories(${COCOA})
endif()

if(UNIX AND NOT APPLE)
	find_package(X11 REQUIRED)
	link_libraries(${X11_LIBRARIES})
	include_directories(${X11_INCLUDE_DIR})
endif()

set(remote-control-ui_HEADERS
	${remote-control-ui_HEADERS}
   command.h
   obscommandhandler.h
   client.h
   commandserver.h
   remotecontroldialog.h
	)
set(remote-control-ui_SOURCES
	${remote-control-ui_SOURCES}
   command.cpp
   obscommandhandler.cpp
   client.cpp
   commandserver.cpp
   remote-control.cpp
   remotecontroldialog.cpp
	)
set(remote-control-ui_UI
	${remote-control-ui_UI}
	forms/output.ui
	)

if(WIN32)
   set(MODULE_DESCRIPTION "OBS Remote Control Server")
	configure_file(${CMAKE_SOURCE_DIR}/cmake/winrc/obs-module.rc.in remote-control-ui.rc)
	list(APPEND remote-control-ui_SOURCES
		remote-control-ui.rc)
endif()

if(APPLE)
	set(remote-control-ui_PLATFORM_LIBS
		${COCOA})
endif()

qt5_wrap_ui(remote-control-ui_UI_HEADERS
	${remote-control-ui_UI})

add_library(remote-control-ui MODULE
	${remote-control-ui_HEADERS}
	${remote-control-ui_SOURCES}
	${remote-control-ui_UI_HEADERS}
	)
target_link_libraries(remote-control-ui
	${frontend-tools_PLATFORM_LIBS}
	obs-frontend-api
	Qt5::Widgets
   Qt5::Network
	libobs)

install_obs_plugin_with_data(remote-control-ui data)
