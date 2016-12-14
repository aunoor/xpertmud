TEMPLATE	= lib
CONFIG		+= qt warn_on plugin thread release exceptions
unix:TARGET	= xmud_battletech

HEADERS		= battletech.h BattleCore.h BattleCoreWidget.h \
		  EasyStyle.h Factory.h HUDParser.h Positions.h \
		  BattleMapWidget.h MechInfo.h BattleWeapons.h \
                  BattleSpeed.h TextParser.h BattleHeat.h BattleStatus.h \
                  BattleContacts.h WeaponInfo.h \
		  HeavyStyle.h \
		  ../../../compat/klibloader.h \
		  ../../../compat/kgenericfactory.h
SOURCES		= battletech.cc BattleCore.cc BattleCoreWidget.cc \
		  Factory.cc HUDParser.cc \
		  BattleMapWidget.cc BattleSpeed.cc BattleHeat.cc BattleStatus.cc \
		  Positions.cc BattleWeapons.cc  MechInfo.cc \
                  BattleContacts.cc WeaponInfo.cc \
		  ../../../compat/kglobal.cc


DEFINES += NO_KDE QT_DLL

INCLUDEPATH += ../../../compat
macx:DEFINES += MACOS_X
win32:target.files=xmud_battletech.dll
target.path=dll/plugins
perlscripts.path=appdata/perl
perlscripts.files=*.pm *.pl
pythonscripts.path=appdata/python
pythonscripts.files=*.py
artwork.path=appdata/artwork
artwork.files=artwork/*.png artwork/CREDITS
artwork_building.path=appdata/artwork/buildings
artwork_building.files=artwork/buildings/building*.png
artwork_road.path=appdata/artwork/roads
artwork_road.files=artwork/roads/road*.png
status.path=appdata/status
status.files=artwork/status/status.def artwork/status/*.png
INSTALLS=target perlscripts pythonscripts status artwork artwork_building artwork_road
