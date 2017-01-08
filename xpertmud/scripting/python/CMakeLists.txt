cmake_minimum_required ( VERSION 3.0.2 )

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

project(xmpythoninterpreter)

file (GLOB HDR
        XMPython.h
        XMPythonFactory.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../Scripting.h
        ${CMAKE_SOURCE_DIR}/compat/klibloader.h
        )

file (GLOB SRCS
        XMPythonFactory.cc
        ${CMAKE_SOURCE_DIR}/compat/kglobal.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/../Scripting.cc
        )

find_package(Qt5Widgets REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(${PYTHON_INCLUDE_DIRS})

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/AutoSysinit.py
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/createCallbacks.py ${CMAKE_CURRENT_SOURCE_DIR}/sysinit_py.in < ${CMAKE_CURRENT_SOURCE_DIR}/../GuiScriptingBindings.h > ${CMAKE_BINARY_DIR}/AutoSysinit.py
        )

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/sysinit_pyf.h
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/py2h.py SYSINIT_PY ${CMAKE_CURRENT_SOURCE_DIR} < ${CMAKE_BINARY_DIR}/AutoSysinit.py > ${CMAKE_BINARY_DIR}/sysinit_pyf.h
        DEPENDS ${CMAKE_BINARY_DIR}/AutoSysinit.py)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/AutoXMPython.cc
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/createCallbacks.py ${CMAKE_CURRENT_SOURCE_DIR}/XMPython_cc.in < ${CMAKE_CURRENT_SOURCE_DIR}/../GuiScriptingBindings.h > ${CMAKE_BINARY_DIR}/AutoXMPython.cc
        DEPENDS ${CMAKE_BINARY_DIR}/sysinit_pyf.h)


add_library(${PROJECT_NAME} SHARED ${SRCS} ${HDR} ${CMAKE_BINARY_DIR}/sysinit_pyf.h ${CMAKE_BINARY_DIR}/AutoXMPython.cc)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} Qt5::Widgets util pthread ${PYTHON_LIBRARY})
else (APPLE)
    target_link_libraries(${PROJECT_NAME} Qt5::Widgets ${PYTHON_LIBRARY})
endif (APPLE)


install(CODE "MESSAGE(\"Python plugin\")")

if (APPLE)
    install(TARGETS ${PROJECT_NAME} DESTINATION ${APP_INSTALL_PATH}/Contents/MacOS/dll/interpreter/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bt3030.py
            ${CMAKE_CURRENT_SOURCE_DIR}/tapp.py
            ${CMAKE_CURRENT_SOURCE_DIR}/ansicolors.py
            DESTINATION ${APP_INSTALL_PATH}/Contents/MacOS/appdata/python/)
else (APPLE)
    if (WIN32)
        install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/dll/interpreter/)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bt3030.py
                ${CMAKE_CURRENT_SOURCE_DIR}/tapp.py
                ${CMAKE_CURRENT_SOURCE_DIR}/ansicolors.py
                DESTINATION ${CMAKE_INSTALL_PREFIX}/appdata/python/)
    else (WIN32)
        #TODO: make Linux install
        install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${MAIN_PROJ_NAME}/dll/interpreter/)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bt3030.py
                ${CMAKE_CURRENT_SOURCE_DIR}/tapp.py
                ${CMAKE_CURRENT_SOURCE_DIR}/ansicolors.py
                DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${MAIN_PROJ_NAME}/appdata/python/)
    endif (WIN32)
endif (APPLE)