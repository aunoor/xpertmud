cmake_minimum_required ( VERSION 2.8.12 )

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

if(POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()

project(xmperlinterpreter)

file (GLOB HDR
        PerlInterpret.h
        XMPerl.h
        XMPerlFactory.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../Scripting.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../GuiScriptingBindings.h
        ${CMAKE_SOURCE_DIR}/compat/klibloader.h
        )

file (GLOB SRCS
        XMPerl.cc
        XMPerlFactory.cc
        ${CMAKE_SOURCE_DIR}/compat/kglobal.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../Scripting.cc
        )

find_package(Qt5Widgets REQUIRED)

if ((CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
#for perl 3.18 we need this
add_definitions(-Wno-reserved-user-defined-literal)
endif((CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${PERL_INCLUDE_PATH})

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/sysinit_plf.h
        COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/pl2h.pl SYSINIT_PL ${CMAKE_CURRENT_SOURCE_DIR} < ${CMAKE_CURRENT_SOURCE_DIR}/sysinit.pl > ${CMAKE_BINARY_DIR}/sysinit_plf.h
        )

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/AutoPerlInterpret.cc
        COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/createCallbacks.pl interpret_cc ${CMAKE_CURRENT_SOURCE_DIR}/PerlInterpret_cc.in < ${CMAKE_CURRENT_SOURCE_DIR}/../GuiScriptingBindings.h > ${CMAKE_BINARY_DIR}/AutoPerlInterpret.cc
        DEPENDS ${CMAKE_BINARY_DIR}/sysinit_plf.h)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/AutoGuiScriptingWrapper.h
        COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/createCallbacks.pl wrapper_h ${CMAKE_CURRENT_SOURCE_DIR}/GuiScriptingWrapper_h.in < ${CMAKE_CURRENT_SOURCE_DIR}/../GuiScriptingBindings.h > ${CMAKE_BINARY_DIR}/AutoGuiScriptingWrapper.h
        )

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/AutoGuiScriptingWrapper.cc
        COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/createCallbacks.pl wrapper_cc ${CMAKE_CURRENT_SOURCE_DIR}/GuiScriptingWrapper_h.in < ${CMAKE_CURRENT_SOURCE_DIR}/../GuiScriptingBindings.h > ${CMAKE_BINARY_DIR}/AutoGuiScriptingWrapper.cc
        DEPENDS ${CMAKE_BINARY_DIR}/sysinit_plf.h)

add_library(${PROJECT_NAME} SHARED ${SRCS} ${HDR} ${CMAKE_BINARY_DIR}/sysinit_plf.h ${CMAKE_BINARY_DIR}/AutoGuiScriptingWrapper.h
            ${CMAKE_BINARY_DIR}/AutoGuiScriptingWrapper.cc ${CMAKE_BINARY_DIR}/AutoPerlInterpret.cc)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets ${PERL_LIBRARY})

install(CODE "MESSAGE(\"Perl plugin\")")

if (APPLE)

    install(TARGETS ${PROJECT_NAME} DESTINATION ${APP_INSTALL_PATH}/Contents/MacOS/dll/interpreter/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/completion.pl
            ${CMAKE_CURRENT_SOURCE_DIR}/snake.pl
            ${CMAKE_CURRENT_SOURCE_DIR}/speedwalk.pl
            ${CMAKE_CURRENT_SOURCE_DIR}/pipes.pl
            ${CMAKE_CURRENT_SOURCE_DIR}/easywalk.pl
            ${CMAKE_CURRENT_SOURCE_DIR}/easywalk.demo.pl
            DESTINATION ${APP_INSTALL_PATH}/Contents/MacOS/appdata/perl/)
else (APPLE)
    if (WIN32)
        install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/dll/interpreter/)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/completion.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/snake.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/speedwalk.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/pipes.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/easywalk.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/easywalk.demo.pl
                DESTINATION ${CMAKE_INSTALL_PREFIX}/appdata/perl/)
    else (WIN32)
        #TODO: make Linux install
        install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${MAIN_PROJ_NAME}/dll/interpreter/)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/completion.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/snake.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/speedwalk.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/pipes.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/easywalk.pl
                ${CMAKE_CURRENT_SOURCE_DIR}/easywalk.demo.pl
                DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${MAIN_PROJ_NAME}/appdata/perl/)
    endif (WIN32)
endif (APPLE)