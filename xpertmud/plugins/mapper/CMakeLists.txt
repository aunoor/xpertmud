cmake_minimum_required ( VERSION 3.0.2 )

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

project(xmud_mapper)

file (GLOB HDR
        GUI/main.hh
        GUI/menu.hh
        GUI/view.hh
        GUI/zonelist.hh
        core/map.hh
        core/mobject.hh
        core/zonemodel.h
        core/core_global.h
        backends/backend.hh
        backends/backend-dummy.hh
        backends/backend-tmw.h
        ${CMAKE_SOURCE_DIR}/compat/klibloader.h
        ${CMAKE_SOURCE_DIR}/compat/kgenericfactory.h
        )

file (GLOB SRCS
        GUI/main.cc
        GUI/menu.cc
        GUI/view.cc
        GUI/zonelist.cc
        core/map.cc
        core/mobject.cc
        core/zonemodel.cc
        backends/backend.cc
        backends/backend-dummy.cc
        backends/backend-tmw.cc
        ${CMAKE_SOURCE_DIR}/compat/kglobal.cc
        )

find_package(Qt5Widgets REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GUI/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/backends/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core/)
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