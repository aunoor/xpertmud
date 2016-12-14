%define prefix  /usr
%define conflags --prefix=%{prefix}

Summary: the xpertmud MUD Client
Name: xpertmud
Version: 3.0
Release: 1
Copyright: GPL
Group: Amusements/Games
BuildRequires: gcc-c++
BuildRequires: libtool automake autoconf
BuildRequires: qt-devel kdelibs-devel perl libart_lgpl-devel
BuildRequires: arts-devel ruby ruby-libs python
BuildRequires: python-devel ruby-devel
BuildRoot: /tmp/xpertmud-buildroot
Requires: kdelibs >= 3.1
Requires: perl >= 5.8.0
Packager: Tuncay Goncuoglu
URL: http://xpertmud.sourceforge.net/

%description
xpertmut MUD CLient, scriptable in Perl, Python and (partially) Ruby


%package scripting-python
Summary: Python scripting package
Group: Amusements/Games
Requires: xpertmud = %{version}-%{release}
Requires: python >= 2.2.2

%description scripting-python
the Python scripting library for xpertmud


%package scripting-ruby
Summary: Ruby scripting package
Group: Amusements/Games
Requires: xpertmud = %{version}-%{release}
Requires: ruby >= 1.6.8
Requires: ruby-libs >= 1.6.8

%description scripting-ruby
the Ruby scripting library for xpertmud


%package plugins-misc
Requires: xpertmud = %{version}-%{release}
Summary: For now the BattleTech plugin
Group: Amusements/Games

%description plugins-misc
misc xpertmud plugins (for now only the battletech hud)


%prep
export CVS_RSH=ssh 
#cvs -z3 -d:ext:snooze@cvs.sourceforge.net:/cvsroot/xpertmud co xpertmud
cvs -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/xpertmud login
cvs -z3 -d:pserver:anonymous@cvs.sourceforge.net:/cvsroot/xpertmud co -D "2004-05-09" xpertmud

%build
cd xpertmud
make -f Makefile.dist
CFLAGS="$RPM_OPT_FLAGS" ./configure %{conflags}
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{prefix}
cd xpertmud
make prefix=$RPM_BUILD_ROOT%{prefix} install


%clean
#rm -rf $RPM_BUILD_ROOT
#cd core/c/lib && make clean
#cd ../server && make clean

%files
%defattr(-,root,root)
%dir %{prefix}/bin
%{prefix}/bin/xpertmud
%{prefix}/lib/kde3/kfile_xmud.la
%{prefix}/lib/kde3/kfile_xmud.so
%{prefix}/lib/kde3/libxmperlinterpreter.la
%{prefix}/lib/kde3/libxmperlinterpreter.so
%{prefix}/lib/kde3/xmud_html.la
%{prefix}/lib/kde3/xmud_html.so
%{prefix}/lib/kde3/xmud_example.la
%{prefix}/lib/kde3/xmud_example.so
%{prefix}/share/applnk/Games/xpertmud.desktop
%dir %{prefix}/share/apps/xpertmud/
%dir %{prefix}/share/apps/xpertmud/artwork/
%{prefix}/share/apps/xpertmud/artwork/*
%dir %{prefix}/share/apps/xpertmud/bookmarks/
%{prefix}/share/apps/xpertmud/bookmarks/*
%dir %{prefix}/share/apps/xpertmud/perl/
%{prefix}/share/apps/xpertmud/perl/XMExample.pm
%{prefix}/share/apps/xpertmud/perl/XMHTML.pm
%{prefix}/share/apps/xpertmud/perl/htmlmapper.pl
%{prefix}/share/apps/xpertmud/perl/pipes.pl
%{prefix}/share/apps/xpertmud/perl/completion.pl
%{prefix}/share/apps/xpertmud/perl/snake.pl
%{prefix}/share/apps/xpertmud/perl/speedwalk.pl
%{prefix}/share/apps/xpertmud/xpertmudui.rc
%{prefix}/share/icons/hicolor/16x16/apps/xpertmud.png
%{prefix}/share/icons/hicolor/32x32/apps/xpertmud.png
%{prefix}/share/icons/locolor/16x16/apps/xpertmud.png
%{prefix}/share/icons/locolor/32x32/apps/xpertmud.png
%{prefix}/share/locale/de/LC_MESSAGES/xpertmud.mo
%{prefix}/share/mimelnk/application/x-xpertmud-bookmark.desktop
%{prefix}/share/services/kfile_xmud.desktop
%{prefix}/share/services/xpertmud.protocol



%files scripting-python
%{prefix}/lib/kde3/libxmpythoninterpreter.la
%{prefix}/lib/kde3/libxmpythoninterpreter.so
%dir %{prefix}/share/apps/xpertmud/python/
%{prefix}/share/apps/xpertmud/python/tapp.py

%files scripting-ruby
%{prefix}/lib/kde3/libxmrubyinterpreter.la
%{prefix}/lib/kde3/libxmrubyinterpreter.so


%files plugins-misc
%{prefix}/lib/kde3/xmud_battletech.la
%{prefix}/lib/kde3/xmud_battletech.so
%{prefix}/share/apps/xpertmud/perl/XMBattleContacts.pm
%{prefix}/share/apps/xpertmud/perl/XMBattleCore.pm
%{prefix}/share/apps/xpertmud/perl/XMBattleHeat.pm
%{prefix}/share/apps/xpertmud/perl/XMBattleMapView.pm
%{prefix}/share/apps/xpertmud/perl/XMBattleSpeed.pm
%{prefix}/share/apps/xpertmud/perl/XMBattleWeapons.pm
%{prefix}/share/apps/xpertmud/perl/xperthud.pl
%dir %{prefix}/share/apps/xpertmud/python/
%{prefix}/share/apps/xpertmud/python/battletech.py
%{prefix}/share/apps/xpertmud/python/bt3030.py
%{prefix}/share/apps/xpertmud/python/btcockpit.py
%{prefix}/share/apps/xpertmud/python/claims.py
%{prefix}/share/apps/xpertmud/python/contacts.py
%{prefix}/share/apps/xpertmud/python/frequencies.py
%{prefix}/share/apps/xpertmud/python/keybindings.py



%changelog
* Tue Nov 12 2003 Peter Triller 
- first rpm package

