cmake_minimum_required ( VERSION 3.0.2 )

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0043)
    cmake_policy(SET CMP0043 NEW)
endif()

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

project(xpertmud)

# Files

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
        xpertmud/config.h
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
        compat/kconfig.h
        compat/kiconloader.h
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
file(GLOB UI_GEN ui_*.h *_automoc.*)

source_group("Autogen files" FILES ${UI_GEN})

set(MAIN_PROJ_NAME ${PROJECT_NAME})
set_property(GLOBAL PROPERTY MCCP_SUPPORT OFF)
#set_property(GLOBAL PROPERTY PYTHON_SUPPORT ON)
#set_property(GLOBAL PROPERTY PERL_SUPPORT ON)
set(MCCP_SUPPORT OFF CACHE BOOL "MCCP protocol support")
set(PYTHON_SUPPORT ON CACHE  BOOL "Python scrypting support")
set(PERL_SUPPORT ON CACHE  BOOL "Perl scrypting support")

if (MCCP_SUPPORT)
    if(WIN32)
        set(ZLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/vendor/zlib-1.2.3)
    endif(WIN32)

    include(FindZLIB)
endif (MCCP_SUPPORT)

if (MCCP_SUPPORT AND (NOT ZLIB_FOUND))
    set(MCCP_SUPPORT OFF)
    message(WARNING "Disable MCCP support - zlib not found")
endif (MCCP_SUPPORT AND (NOT ZLIB_FOUND))

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/xpertmud/config.h CURRENT_CONFIG)
string(REGEX MATCH "\#define APP_VERSION[ ]+[\"]?[a-z0-9.]+[\"]?" APP_VERSION_DEF ${CURRENT_CONFIG})
string(STRIP ${APP_VERSION_DEF} APP_VERSION_DEF)
string(REPLACE "\#define APP_VERSION" "" APP_VERSION_DEF ${APP_VERSION_DEF})
string(REPLACE "\"" "" APP_VERSION_DEF ${APP_VERSION_DEF})
string(STRIP ${APP_VERSION_DEF} APP_VERSION_DEF)
string(REGEX MATCH "[0-9.]+"  APP_VERSION_SHORT ${APP_VERSION_DEF})
message("Version of application is ${APP_VERSION_DEF}, short is ${APP_VERSION_SHORT}")

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


find_package(Qt5Gui REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
#find_package(Qt5XmlPatterns)
find_package(Qt5Network REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

if (Qt5Widgets_FOUND)
    if (Qt5Widgets_VERSION VERSION_LESS 5.6.0)
        message(FATAL_ERROR "Minimum supported Qt5 version is 5.6.0!")
    endif()
endif(Qt5Widgets_FOUND)

include_directories( ${Qt5Network_INCLUDE_DIRS} )
include_directories( ${Qt5Concurrent_INCLUDE_DIRS} )
include_directories( ${Qt5Multimedia_INCLUDE_DIRS} )

SET(CMAKE_AUTOMOC TRUE)
#SET(CMAKE_AUTOMOC_RELAXED_MODE TRUE)

if(MSVC)
	add_definitions(-DNOMINMAX)
	add_definitions(/FR)
	add_definitions(-D_USE_MATH_DEFINES)
	add_definitions( /wd4786 /wd4503 /wd4251 /wd4275 /wd4290 /wd4661 /wd4996 /wd4127 /wd4100)
elseif(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
	#add_definitions (-Wall -fPIC)
    add_definitions (-Wall)
endif(MSVC)

add_definitions(-DQT_DLL)

add_definitions(-DNO_KDE)
add_definitions(-DNO_KDE2)
add_definitions(-DNO_INCLUDE_MOCFILES)
add_definitions(-DMAKEDLL_QEXTMDI)

if (MCCP_SUPPORT)
    add_definitions(-DMCCP_SUPPORT)
endif(MCCP_SUPPORT)

qt5_wrap_ui(INCLUDE_LIST_FORMS ${UI_FORMS})
qt5_add_resources(MAIN_UI_RC ${MAIN_RES})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/xpertmud)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/compat)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qextmdi/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qextmdi/res)
if (MCCP_SUPPORT)
    include_directories(${ZLIB_INCLUDE_DIRS})
endif(MCCP_SUPPORT)

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
    if (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
	    set(GUI_TYPE WIN32)
    endif (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
elseif(APPLE)
	set(GUI_TYPE MACOSX_BUNDLE)

    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks;@executable_path/../Frameworks")
#    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")

endif(WIN32)

if("${CMAKE_SYSTEM}" MATCHES "Linux")
    if(CMAKE_COMPILER_IS_GNUCXX)
	#RPATH work for standalone install
#	set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
#	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
#	set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")
    endif()
endif()

add_executable(${PROJECT_NAME} ${GUI_TYPE} ${SRCS} ${HDR} ${RCC_FILE} ${INCLUDE_LIST_FORMS} ${MAIN_UI_RC})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Xml Qt5::Network Qt5::Multimedia)
if (MCCP_SUPPORT)
    target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
endif(MCCP_SUPPORT)

install(CODE "MESSAGE(\"Main app\")")

if (APPLE)

    set(APP_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app")

    function(QT_MODULE_DEPLOY QMODULE_NAME)
        string(REPLACE ":" "" QMODULE_NAME ${QMODULE_NAME})
        set(QMODULE_NAME "_${QMODULE_NAME}_OWN_INCLUDE_DIRS")
#        message(${QMODULE_NAME})
#list(GET _Qt5Widgets_OWN_INCLUDE_DIRS 0 QM_PATH)
        list(GET ${QMODULE_NAME} 0 QM_PATH)
#        message(${QM_PATH})
        #string(FIND ${QM_PATH} "\/" <output variable> REVERSE)
        string(STRIP ${QM_PATH} QM_PATH)
        string(REGEX MATCH "Qt[^/].+framework$" QM_F_NAME ${QM_PATH})
        string(REPLACE ".framework" "" QM_NAME ${QM_F_NAME})
#        message("${QM_PATH} | ${QM_F_NAME} | ${QM_NAME}")

#        if (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
#            set(QM_NAME "${QM_NAME}_debug")
#        endif (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")

        install(FILES
                ${QM_PATH}/${QM_NAME}
                DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks/${QM_F_NAME})
        install(FILES ${QM_PATH}/Resources/Info.plist
                DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks/${QM_F_NAME}/Versions/5/Resources/)
        install(FILES ${QM_PATH}/Versions/5/${QM_NAME}
                DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks/${QM_F_NAME}/Versions/5/)
        install(DIRECTORY ${QM_PATH}/Versions/Current
                DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Frameworks/${QM_F_NAME}/Versions/)

#dirty, dirty hack... May be someone has better variant?
        install(FILES ${QM_PATH}/../../plugins/platforms/libqcocoa.dylib
                DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/PlugIns/platforms)
    endfunction()

    set(MACOSX_BUNDLE_BUNDLE_NAME "Xpertmud")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "org.xpertmud.Xpertmud")
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${APP_VERSION_SHORT})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${APP_VERSION_SHORT})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${APP_VERSION_DEF})
    set(MACOSX_BUNDLE_ICON_FILE "hi32-app-xpertmud.icns")
    set(MACOSX_BUNDLE_INFO_STRING "Xpertmud is an extensible scriptable MUD client")

    install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qt.conf.macos
            DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Resources/ RENAME qt.conf)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/xpertmud/hi32-app-xpertmud.icns
            DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/Resources/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/xpertmud/hi32-app-xpertmud.png
                  ${CMAKE_CURRENT_SOURCE_DIR}/xpertmud/hi16-app-xpertmud.png
            DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/MacOS/icons/)

    install(CODE "MESSAGE(\"Qt framework\")")
    #yes, we can do it as for WIN, but why not play a little?
    QT_MODULE_DEPLOY(Qt5::Core)
    QT_MODULE_DEPLOY(Qt5::Gui)
    QT_MODULE_DEPLOY(Qt5::Widgets)
    QT_MODULE_DEPLOY(Qt5::Xml)
    QT_MODULE_DEPLOY(Qt5::Network)
    QT_MODULE_DEPLOY(Qt5::Multimedia)
#PrintSupport needed for qcocoa platform plugin
    QT_MODULE_DEPLOY(Qt5::PrintSupport)

else (APPLE)
  if (WIN32)
      install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/)
      install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/xpertmud/hi32-app-xpertmud.png
              ${CMAKE_CURRENT_SOURCE_DIR}/xpertmud/hi16-app-xpertmud.png
              DESTINATION ${CMAKE_INSTALL_PREFIX}/icons/)

      if (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
            install(FILES ${QTDIR}/bin/Qt5Widgetsd.dll
                    ${QTDIR}/bin/Qt5Cored.dll
                    ${QTDIR}/bin/Qt5Guid.dll
                    ${QTDIR}/bin/Qt5Multimediad.dll
                    ${QTDIR}/bin/Qt5Networkd.dll
                    ${QTDIR}/bin/Qt5Xmld.dll
                    DESTINATION ${CMAKE_INSTALL_PREFIX}/)
            install(FILES ${QTDIR}/plugins/platforms/qwindowsd.dll
                    DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins/platforms/)
      else(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
            install(FILES ${QTDIR}/bin/Qt5Widgets.dll
                    ${QTDIR}/bin/Qt5Core.dll
                    ${QTDIR}/bin/Qt5Gui.dll
                    ${QTDIR}/bin/Qt5Multimedia.dll
                    ${QTDIR}/bin/Qt5Network.dll
                    ${QTDIR}/bin/Qt5Xml.dll
                    DESTINATION ${CMAKE_INSTALL_PREFIX}/)
            install(FILES ${QTDIR}/plugins/platforms/qwindows.dll
                    DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins/platforms/)
      endif (CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")

      install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qt.conf.macos
              DESTINATION ${CMAKE_INSTALL_PREFIX}/ RENAME qt.conf)

      if(CMAKE_COMPILER_IS_GNUCXX)
        install(FILES ${QTDIR}/bin/libgcc_s_dw2-1.dll
                ${QTDIR}/bin/libstdc++-6.dll
                ${QTDIR}/bin/libwinpthread-1.dll
                DESTINATION ${CMAKE_INSTALL_PREFIX}/)
      endif(CMAKE_COMPILER_IS_GNUCXX)

  else (WIN32)
#TODO: make Linux install
      install(TARGETS ${PROJECT_NAME} DESTINATION bin)
      install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/xpertmud/hi32-app-xpertmud.png
              ${CMAKE_CURRENT_SOURCE_DIR}/xpertmud/hi16-app-xpertmud.png
              DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${MAIN_PROJ_NAME}/icons/)
  endif (WIN32)
endif (APPLE)

add_subdirectory(xpertmud/scripting)
add_subdirectory(xpertmud/plugins)

