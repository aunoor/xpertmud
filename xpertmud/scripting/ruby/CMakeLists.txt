cmake_minimum_required ( VERSION 2.8.12 )

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

project(xmrubyinterpreter)

include(FindRuby)

file (GLOB HDR
        XMRuby.h
        XMRubyFactory.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../Scripting.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../GuiScriptingBindings.h
        ${CMAKE_SOURCE_DIR}/compat/klibloader.h
        )

file (GLOB SRCS
        XMRubyFactory.cc
        ${CMAKE_SOURCE_DIR}/compat/kglobal.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../Scripting.cc
        )

find_package(Qt5Widgets REQUIRED)

if ((CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
#for perl 3.18 we need this
#add_definitions(-Wno-reserved-user-defined-literal)
endif((CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${RUBY_INCLUDE_PATH})

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/sysinit_rb.h
        COMMAND ${RUBY_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/rb2h.rb SYSINIT < ${CMAKE_CURRENT_SOURCE_DIR}/sysinit.rb > ${CMAKE_BINARY_DIR}/sysinit_rb.h)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/AutoXMRuby.cc
        COMMAND ${RUBY_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/createCallbacks.rb ${CMAKE_CURRENT_SOURCE_DIR}/XMRuby_cc.in < ${CMAKE_CURRENT_SOURCE_DIR}/../GuiScriptingBindings.h > ${CMAKE_BINARY_DIR}/AutoXMRuby.cc
        DEPENDS ${CMAKE_BINARY_DIR}/sysinit_rb.h)

add_library(${PROJECT_NAME} SHARED ${SRCS} ${HDR} ${CMAKE_BINARY_DIR}/sysinit_rb.h ${CMAKE_BINARY_DIR}/AutoXMRuby.cc)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets ${RUBY_LIBRARY})

install(CODE "MESSAGE(\"Ruby plugin\")")

if (APPLE)

    install(TARGETS ${PROJECT_NAME} DESTINATION ${APP_INSTALL_PATH}/Contents/MacOS/dll/interpreter/)

else (APPLE)
    if (WIN32)
        install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/dll/interpreter/)

    else (WIN32)

        install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${MAIN_PROJ_NAME}/dll/interpreter/)

    endif (WIN32)
endif (APPLE)