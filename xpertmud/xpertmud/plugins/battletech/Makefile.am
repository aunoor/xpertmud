SUBDIRS=. artwork xperthud
# xperthud in here, because configure doesnt work 
# skipped subdir: xperthud  TODO: fix it first

kde_module_LTLIBRARIES = xmud_battletech.la

xmud_battletech_la_SOURCES=BattleCore.cc BattleCoreWidget.cc \
	HUDParser.cc BattleMapWidget.cc BattleContacts.cc \
	battletech.cc Factory.cc Positions.cc MechInfo.cc \
	BattleHeat.cc BattleWeapons.cc \
	BattleSpeed.cc WeaponInfo.cc BattleStatus.cc

#	TextParser.cc 

xmud_battletech_la_METASOURCES = AUTO

xmud_battletech_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
xmud_battletech_la_LIBADD = $(LIB_KIO) $(LIB_KDECORE)

# TODO: Remove -g as its an evil hack. set CXXFLAGS before calling ./configure
INCLUDES= -g $(all_includes)

EXTRA_DIST=battletech.h XMBattleMapView.pm XMBattleCore.pm XMBattleHeat.pm \
	XMBattleWeapons.pm

perldir = $(kde_datadir)/xpertmud/perl
perl_DATA = XMBattleMapView.pm XMBattleCore.pm XMBattleStatus.pm \
	XMBattleContacts.pm XMBattleHeat.pm XMBattleSpeed.pm \
	XMBattleWeapons.pm xperthud.pl htmlmapper.pl battlerecorder.pl \
	damagelog.pl bt3030.pl frontiers.pl

pythondir = $(kde_datadir)/xpertmud/python
python_DATA = battletech.py keybindings.py btcockpit.py \
	frequencies.py claims.py contacts.py
