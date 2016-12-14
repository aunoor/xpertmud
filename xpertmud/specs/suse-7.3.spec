#
# spec file for package xpertmud (Version 2.0beta3)
#

Distribution: SuSE Linux 7.3 (i386)
Name:         xpertmud
Copyright:    GPL
Requires:     klibs2
Autoreqprov:  on
Group:        X11/KDE/Games
Summary:      Perl and Python scriptable MUD Client
Version:      2.0beta3
Release:      0
Source:       xpertmud-2.0beta3.tar.gz
Prefix:       /opt/kde2
BuildRoot: /var/tmp/%{name}-buildroot

%description
this is xpertmud, the xtensible PERL-Scriptable MUD Client.

Authors:
--------

      Ernst Bachmann <e.bachmann@xebec.de>
      Manuel Klimek <manuel.klimek@web.de>

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q

%build

CXXFLAGS="$CXXFLAGS $RPM_OPT_FLAGS " ./configure \
  --prefix=/opt/kde2 \
 --mandir=/usr/share/man/ \
 --with-qt-dir=/usr/lib/qt2 \
 --with-install-root=$RPM_BUILD_ROOT 

make
%install
make install DESTDIR=$RPM_BUILD_ROOT


%files
%doc TODO README AUTHORS INSTALL INSTALL-LOCAL ChangeLog COPYING 
/opt/kde2/bin/xpertmud
%dir /opt/kde2/share/doc/HTML/en/xpertmud
/opt/kde2/share/doc/HTML/en/xpertmud/common
/opt/kde2/share/doc/HTML/en/xpertmud/index.cache.bz2
/opt/kde2/share/doc/HTML/en/xpertmud/index.docbook
%dir /opt/kde2/share/apps/xpertmud
%dir /opt/kde2/share/apps/xpertmud/perl
/opt/kde2/share/apps/xpertmud/perl/bt3030.pl
/opt/kde2/share/apps/xpertmud/perl/snake.pl
%dir /opt/kde2/share/apps/xpertmud/python
/opt/kde2/share/apps/xpertmud/python/bt3030.py
/opt/kde2/share/apps/xpertmud/xpertmudui.rc
/opt/kde2/share/icons/locolor/16x16/apps/xpertmud.png
/opt/kde2/share/icons/locolor/32x32/apps/xpertmud.png
/opt/kde2/share/icons/hicolor/16x16/apps/xpertmud.png
/opt/kde2/share/icons/hicolor/32x32/apps/xpertmud.png
/opt/kde2/share/applnk/Games/xpertmud.desktop
%dir /opt/kde2/lib/kde2
/opt/kde2/lib/kde2/libxmpythoninterpreter.la
/opt/kde2/lib/kde2/libxmpythoninterpreter.so
/opt/kde2/lib/kde2/libxmperlinterpreter.la
/opt/kde2/lib/kde2/libxmperlinterpreter.so
/opt/kde2/share/locale/de/LC_MESSAGES/xpertmud.mo
