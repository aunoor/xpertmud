cmake_minimum_required ( VERSION 2.8.12 )

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0043)
    cmake_policy(SET CMP0043 NEW)
endif()

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

project(xpertmud-ng)

# Files

#file(GLOB HDR xpertmud/*.h
#		      compat/*.h
#	)

file (GLOB HDR
xpertmud/TextBuffer.h
xpertmud/TextBufferView.h
xpertmud/TextBufferHistoryView.h
xpertmud/mxinputline.h
xpertmud/xpertmud.h
xpertmud/InputAction.h
xpertmud/scripting/Scripting.h
xpertmud/PluginWrapper.h
xpertmud/configdialog.h
xpertmud/ColorChar.h
xpertmud/Connection.h
xpertmud/ConnectionHandler.h
xpertmud/IacHandlerCollection.h
xpertmud/CtrlParser.h
xpertmud/TelnetFilter.h
xpertmud/InputStreamHandler.h
xpertmud/Bookmark.h
xpertmud/BookmarkEditor.h
xpertmud/LogToFile.h
xpertmud/MCCP.h
compat/kaction.h
compat/kfontdialog.h
compat/kapplication.h
compat/kcolorbutton.h
compat/kdialogbase.h
compat/kextendedsocket.h
compat/kfiledialog.h
compat/klibloader.h
compat/kmainwindow.h
compat/kmessagebox.h
compat/kpopupmenu.h
compat/ktoolbar.h
compat/dummykpartsdockmainwindow.h
compat/kdocktabctl.h
compat/kdockwidget.h
compat/kdockwidget_private.h
compat/kstandarddirs.h
compat/arpa/telnet.h
qextmdi/include/qextmditaskbar.h
qextmdi/include/qextmdichildarea.h
qextmdi/include/qextmdichildfrmcaption.h
qextmdi/include/qextmdichildfrm.h
qextmdi/include/qextmdichildview.h
qextmdi/include/qextmdidefines.h
qextmdi/include/qextmdiiterator.h
qextmdi/include/qextmdilistiterator.h
qextmdi/include/qextmdinulliterator.h
qextmdi/include/qextmdimainfrm.h
qextmdi/res/*.xpm
)

#file(GLOB SRCS xpertmud/*.cc
#		       compat/*.cc
#		       compat/*.cpp
#	)

file (GLOB SRCS
compat/klibloader.cc
compat/kapplication.cc
compat/kglobal.cc
compat/kdocktabctl.cpp
compat/kmainwindow.cc
compat/kdockwidget.cpp
compat/kdockwidget_private.cpp
compat/ktoolbar.cpp
compat/kstandarddirs.cpp
qextmdi/src/qextmdichildarea.cpp
qextmdi/src/qextmdichildfrmcaption.cpp
qextmdi/src/qextmdichildfrm.cpp
qextmdi/src/qextmdichildview.cpp
qextmdi/src/qextmdimainfrm.cpp
qextmdi/src/qextmditaskbar.cpp
xpertmud/TextBuffer.cc
xpertmud/TextBufferView.cc
xpertmud/TextBufferHistoryView.cc
xpertmud/mxinputline.cc
xpertmud/InputAction.cc
xpertmud/xpertmud.cc
xpertmud/main.cc
xpertmud/xpertmud_scripting.cc
xpertmud/PluginWrapper.cc
xpertmud/Connection.cc
xpertmud/ConnectionHandler.cc
xpertmud/IacHandlerCollection.cc
xpertmud/CtrlParser.cc
xpertmud/TelnetFilter.cc
xpertmud/InputStreamHandler.cc
xpertmud/configdialog.cc
xpertmud/LogToFile.cc
xpertmud/Bookmark.cc
xpertmud/BookmarkEditor.cc
xpertmud/MCCP.cc
xpertmud/scripting/Scripting.cc
)

#file(GLOB UI_FORMS src/ui/*.ui)
#file(GLOB MAIN_RES src/*.qrc)
file(GLOB UI_GEN ui_*.h moc_*)

#source_group("Forms" FILES ${UI_FORMS})
source_group("Autogen files" FILES ${UI_GEN})
set(CMAKE_AUTOMOC ON)

set(MAIN_PROJ_NAME ${PROJECT_NAME})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY MCCP_SUPPORT OFF)

set(MACOSX_BUNDLE_GUI_IDENTIFIER "XPM.\${PRODUCT_NAME:identifier}")

if (WIN32 OR APPLE)
    set(QTDIR "" CACHE PATH "Qt root dir")
    set(CMAKE_PREFIX_PATH
	    ${QTDIR}
	    ${QTDIR}/bin
	    ${QTDIR}/lib
    	${CMAKE_PREFIX_PATH}
    )
    set ( CMAKE_MODULE_PATH
        ${QTDIR}/lib/cmake
        ${CMAKE_MODULE_PATH}
    )
endif(WIN32 OR APPLE)


find_package(Qt5Gui)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5XmlPatterns)
find_package(Qt5Network REQUIRED)
#find_package(Qt5Sql REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Multimedia REQUIRED)
#find_package(Qt5PrintSupport REQUIRED)

include_directories( ${Qt5Svg_INCLUDE_DIRS} )
#include_directories( ${Qt5PrintSupport_INCLUDE_DIRS} )
include_directories( ${Qt5Network_INCLUDE_DIRS} )
include_directories( ${Qt5Concurrent_INCLUDE_DIRS} )
include_directories( ${Qt5Multimedia_INCLUDE_DIRS} )
#include_directories( ${Qt5Sql_INCLUDE_DIRS} )

SET(CMAKE_AUTOMOC TRUE)
#SET(CMAKE_AUTOMOC_RELAXED_MODE TRUE)

if(MSVC)
	add_definitions(-DNOMINMAX)
	add_definitions(/FR)
	add_definitions(-D_USE_MATH_DEFINES)
	add_definitions( /wd4786 /wd4503 /wd4251 /wd4275 /wd4290 /wd4661 /wd4996 /wd4127 /wd4100)
elseif(CMAKE_COMPILER_IS_GNUCXX)
	#add_definitions (-Wall -fPIC)
    add_definitions (-Wall)
endif(MSVC)

add_definitions(-DNO_KDE)
add_definitions(-DNO_KDE2)
add_definitions(-DNO_INCLUDE_MOCFILES)
add_definitions(-DMAKEDLL_QEXTMDI)
if (APPLE)
    add_definitions(-DMACOS_X)
endif(APPLE)

if (MCCP_SUPPORT)
    add_definitions(-DMCCP_SUPPORT)
endif(MCCP_SUPPORT)

set(LIBRARY_OUTPUT_PATH ${FIELD_WORK_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${FIELD_WORK_BINARY_DIR})

qt5_wrap_ui(INCLUDE_LIST_FORMS ${UI_FORMS})
qt5_add_resources(MAIN_UI_RC ${MAIN_RES})


include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/compat)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qextmdi/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qextmdi/res)

# Build

if(MSVC)
else()
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    endif(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(MSVC)

if(WIN32)
	set(GUI_TYPE WIN32)
elseif(APPLE)
	set(GUI_TYPE MACOSX_BUNDLE)
endif(WIN32)

if("${CMAKE_SYSTEM}" MATCHES "Linux")
    if(CMAKE_COMPILER_IS_GNUCXX)
	#RPATH work
	set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
	set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")
	
	#set(CMAKE_SKIP_RPATH TRUE)
	#set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath=$ORIGIN/lib")
    endif()
endif()

add_executable(${PROJECT_NAME} ${GUI_TYPE} ${SRCS} ${HDR} ${RCC_FILE} ${INCLUDE_LIST_FORMS} ${MAIN_UI_RC})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Xml Qt5::Network Qt5::Multimedia)
if (MCCP_SUPPORT)
    target_link_libraries(${PROJECT_NAME} z)
endif(MCCP_SUPPORT)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})

if (APPLE)
#  install(DIRECTORY ${MARBLE_LIBRARIES} DESTINATION ${CMAKE_INSTALL_PREFIX}${PROJECT_NAME}.app/Contents/MacOs
#          FILES_MATCHING PATTERN "lib*")
#  install(DIRECTORY ${MARBLE_DIR}/share/marble/data DESTINATION ${CMAKE_INSTALL_PREFIX}${PROJECT_NAME}.app/Contents/MacOs/resources/)
else (APPLE)
  if (WIN32)
#      install(DIRECTORY ${MARBLE_LIBRARIES}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/
#              FILES_MATCHING PATTERN "*.dll")
#      install(DIRECTORY ${GEOGRAPHIC_LIBRARIES}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/
#              FILES_MATCHING PATTERN "*.dll")
  else (WIN32)
#      install(DIRECTORY ${MARBLE_LIBRARIES} DESTINATION ${CMAKE_INSTALL_PREFIX}/
#        FILES_MATCHING PATTERN "lib*"
#        PATTERN "marble" EXCLUDE
#        )
#      install(DIRECTORY ${GEOGRAPHIC_LIBRARIES} DESTINATION ${CMAKE_INSTALL_PREFIX}/
#        FILES_MATCHING PATTERN "lib*"
#        PATTERN "python" EXCLUDE
#        )
  endif (WIN32)
#  install(DIRECTORY ${MARBLE_DIR}/share/marble/data DESTINATION ${CMAKE_INSTALL_PREFIX}/resources/)
#  install(FILES ${INSTALL_PLUGLIST} DESTINATION ${CMAKE_INSTALL_PREFIX}/resources/plugins)
endif (APPLE)


#install(FILES ${MARBLE_LIBRARIES}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/field_x/)

#SETUP_APPLICATION(${PROJECT_NAME})
#add_subdirectory(plugins)
add_subdirectory(xpertmud/scripting)
