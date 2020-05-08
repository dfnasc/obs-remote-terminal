project(remote-terminal)

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

set(remote-terminal_HEADERS
	${remote-terminal_HEADERS}
   command.h
   obscommandhandler.h
   client.h
   commandserver.h
   remotecontroldialog.h
	)
set(remote-terminal_SOURCES
	${remote-terminal_SOURCES}
   rt-plugin.cpp
   remotecontroldialog.cpp
   commandserver.cpp
   command.cpp
   obscommandhandler.cpp
   client.cpp
	)
set(remote-terminal_UI
	${remote-terminal_UI}
	forms/output.ui
	)

if(WIN32)
   set(MODULE_DESCRIPTION "OBS Remote Control Server")
	configure_file(${CMAKE_SOURCE_DIR}/cmake/winrc/obs-module.rc.in remote-terminal.rc)
	list(APPEND remote-terminal_SOURCES
		remote-terminal.rc)
endif()

if(APPLE)
	set(remote-terminal_PLATFORM_LIBS
		${COCOA})
endif()

qt5_wrap_ui(remote-terminal_UI_HEADERS
	${remote-terminal_UI})

add_library(remote-terminal MODULE
	${remote-terminal_HEADERS}
	${remote-terminal_SOURCES}
	${remote-terminal_UI_HEADERS}
	)
target_link_libraries(remote-terminal
	${frontend-tools_PLATFORM_LIBS}
	obs-frontend-api
	Qt5::Widgets
   Qt5::Network
	libobs)

install_obs_plugin_with_data(remote-terminal data)
