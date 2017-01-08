cmake_minimum_required ( VERSION 3.0.2 )

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

project(xmud_mapper)

file (GLOB HDR
        MapperCanvas.hh
        MapperGUI.hh
        MapperMenu.hh
        MapperRoom.hh
        MapperZoneBox.hh
        ${CMAKE_SOURCE_DIR}/compat/klibloader.h
        ${CMAKE_SOURCE_DIR}/compat/kgenericfactory.h
        )

file (GLOB SRCS
        MapperCanvas.cc
        MapperGUI.cc
        MapperMenu.cc
        MapperRoom.cc
        MapperZoneBox.cc
        ${CMAKE_SOURCE_DIR}/compat/kglobal.cc
        )

find_package(Qt5Widgets REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

add_library(${PROJECT_NAME} SHARED ${SRCS} ${HDR})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

install(CODE "MESSAGE(\"Mapper plugin\")")

if (APPLE)
    install(TARGETS ${PROJECT_NAME} DESTINATION ${APP_INSTALL_PATH}/Contents/MacOS/dll/plugins/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/XMMapper.py
            DESTINATION ${APP_INSTALL_PATH}/Contents/MacOS/appdata/python/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/XMMapper.pm
            DESTINATION ${APP_INSTALL_PATH}/Contents/MacOS/appdata/perl/)
else (APPLE)
    if (WIN32)
        install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/dll/plugins/)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/XMMapper.py
                DESTINATION ${CMAKE_INSTALL_PREFIX}/appdata/python/)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/XMMapper.pm
                DESTINATION ${CMAKE_INSTALL_PREFIX}/appdata/perl/)
    else (WIN32)
        install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${MAIN_PROJ_NAME}/dll/plugins/)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/XMMapper.py
                DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${MAIN_PROJ_NAME}/appdata/python/)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/XMMapper.pm
                DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${MAIN_PROJ_NAME}/appdata/perl/)
    endif (WIN32)
endif (APPLE)