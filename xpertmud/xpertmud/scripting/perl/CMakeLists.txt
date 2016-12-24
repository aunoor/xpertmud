cmake_minimum_required ( VERSION 2.8.12 )

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

project(xmperlinterpreter)

#we use Python 2.x
#set(Python_ADDITIONAL_VERSIONS 2.7 2.6 2.5 2.4)

#include(FindPythonLibs)
#include(FindPythonInterp)

include(FindPerlLibs)
include(FindPerl)

#for manual link against Python use PYTHON_LIBRARY and PYTHON_INCLUDE_DIR

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

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${PERL_INCLUDE_DIRS})

#add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/AutoSysinit.py
#        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/createCallbacks.py ${CMAKE_CURRENT_SOURCE_DIR}/sysinit_py.in < ${CMAKE_CURRENT_SOURCE_DIR}/../GuiScriptingBindings.h > ${CMAKE_BINARY_DIR}/AutoSysinit.py
#        )
#
#add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/sysinit_pyf.h
#        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/py2h.py SYSINIT_PY ${CMAKE_CURRENT_SOURCE_DIR} < ${CMAKE_BINARY_DIR}/AutoSysinit.py > ${CMAKE_BINARY_DIR}/sysinit_pyf.h
#        DEPENDS ${CMAKE_BINARY_DIR}/AutoSysinit.py)
#
#add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/AutoXMPython.cc
#        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/createCallbacks.py ${CMAKE_CURRENT_SOURCE_DIR}/XMPython_cc.in < ${CMAKE_CURRENT_SOURCE_DIR}/../GuiScriptingBindings.h > ${CMAKE_BINARY_DIR}/AutoXMPython.cc
#        DEPENDS ${CMAKE_BINARY_DIR}/sysinit_pyf.h)


add_library(${PROJECT_NAME} SHARED ${SRCS} ${HDR} ${CMAKE_BINARY_DIR}/AutoGuiScriptingWrapper.h ${CMAKE_BINARY_DIR}/AutoGuiScriptingWrapper.cc ${CMAKE_BINARY_DIR}/AutoPerlInterpret.cc)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets ${PERL_LIBRARY})
