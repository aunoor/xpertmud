cmake_minimum_required ( VERSION 2.8.12 )

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

project(xmud_battletech)

file (GLOB HDR
        battletech.h
        BattleCore.h
        BattleCoreWidget.h
        EasyStyle.h
        Factory.h
        HUDParser.h
        Positions.h
        BattleMapWidget.h
        MechInfo.h
        BattleWeapons.h
        BattleSpeed.h
        BattleHeat.h
        BattleStatus.h
        BattleContacts.h
        WeaponInfo.h
        HeavyStyle.h
        ${CMAKE_SOURCE_DIR}/compat/klibloader.h
        ${CMAKE_SOURCE_DIR}/compat/kgenericfactory.h
        )

file (GLOB SRCS
        battletech.cc
        BattleCore.cc
        BattleCoreWidget.cc
        Factory.cc
        HUDParser.cc
        BattleMapWidget.cc
        BattleSpeed.cc
        BattleHeat.cc
        BattleStatus.cc
        Positions.cc
        MechInfo.cc
        WeaponInfo.cc
        BattleContacts.cc
        BattleWeapons.cc
        ${CMAKE_SOURCE_DIR}/compat/kglobal.cc
        )

find_package(Qt5Widgets REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

add_library(${PROJECT_NAME} SHARED ${SRCS} ${HDR})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

