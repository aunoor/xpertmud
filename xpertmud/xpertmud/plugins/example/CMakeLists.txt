cmake_minimum_required ( VERSION 2.8.12 )

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

project(xmud_example)

file (GLOB HDR
        example.h
        ${CMAKE_SOURCE_DIR}/compat/klibloader.h
        ${CMAKE_SOURCE_DIR}/compat/kgenericfactory.h
        )

file (GLOB SRCS
        example.cc
        ${CMAKE_SOURCE_DIR}/compat/kglobal.cc
        )

find_package(Qt5Widgets REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

add_library(${PROJECT_NAME} SHARED ${SRCS} ${HDR})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

install(CODE "MESSAGE(\"Example plugin\")")

if (APPLE)
    install(TARGETS ${PROJECT_NAME} DESTINATION ${APP_INSTALL_PATH}/Contents/MacOS/dll/plugins/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/XMExample.py
            DESTINATION ${APP_INSTALL_PATH}/Contents/MacOS/appdata/python/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/XMExample.pm
            DESTINATION ${APP_INSTALL_PATH}/Contents/MacOS/appdata/perl/)
else (APPLE)
    if (WIN32)
        install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/dll/plugins/)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/XMExample.py
                DESTINATION ${CMAKE_INSTALL_PREFIX}/appdata/python/)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/XMExample.pm
                DESTINATION ${CMAKE_INSTALL_PREFIX}/appdata/perl/)
    else (WIN32)
        #TODO: make Linux install
        install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${MAIN_PROJ_NAME}/dll/plugins/)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/XMExample.py
                DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${MAIN_PROJ_NAME}/appdata/python/)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/XMExample.pm
                DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${MAIN_PROJ_NAME}/appdata/perl/)
    endif (WIN32)
endif (APPLE)