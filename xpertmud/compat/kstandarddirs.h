#ifndef KSTANDARDDIRS_H
#define KSTANDARDDIRS_H

#ifdef WIN32
#pragma warning( disable : 4100 )
#endif

#include "kurl.h"
#include <qstringlist.h>
#include <qdir.h>
#include <qregexp.h>
#include <qapplication.h>
#include <iostream>
using std::cout;
using std::endl;

class KStandardDirs {
 public:
  QString findResource(const QString& type, const QString& name) {
    if(type == "appdata") {
      return type + '/' + name;
    }
    return name;
  }

  QStringList findAllResources(const QString& type,
		   const QString& cfilter,
		   bool, bool) {
    cout << "Searching for: " << type.latin1() << "/" << cfilter.latin1() << endl;
    QStringList ret;
    if(type == "module") {
      findModules("dll/interpreter", cfilter, ret);
      findModules("dll/plugins", cfilter, ret);
      findModules("dll/interpreter", "lib"+cfilter, ret);
      findModules("dll/plugins", "lib"+cfilter, ret);
    } else if(type == "icon") {
      QDir d("icons");
      d.setFilter( QDir::Files | QDir::Readable );

      QString filter = cfilter;
      const QFileInfoList *fList = d.entryInfoList(filter);

      if(fList == NULL) return ret;

      QFileInfoListIterator it( *fList );
      QFileInfo *fi;
      while ( (fi=it.current()) ) {
	QString name = fi->filePath();
	cout << name.latin1() << endl;
	ret.append(name);
	++it;
      }
    }
    return ret;
  }

  QStringList findDirs(const QString& type, const QString& name) {
    QStringList ret;
    if(type == "appdata") {
      if(name == "perl") {
	ret.append("appdata/perl");
      } else if(name == "python") {
	ret.append("appdata/python");
      }
    }
    return ret;
  }

  void findModules(const QString& dir, const QString &cfilter, 
		   QStringList& ret) {
      QDir d(dir);
      d.setFilter( QDir::Files | QDir::Readable );

      QString filter = cfilter;
      bool isDotLa = false;
      if(filter.find(".la") != -1) {
#ifdef WIN32
	filter.replace(QRegExp("\\.la"), ".dll");
#else
#ifdef MACOS_X
	filter.replace(QRegExp("\\.la"), ".dylib");
#else
	filter.replace(QRegExp("\\.la"), ".so");
#endif
#endif
	isDotLa = true;
      }
	cout << "Final Filter: " << filter.latin1() << endl;
      const QFileInfoList *fList = d.entryInfoList(filter);

      if(fList == NULL) return;

      QFileInfoListIterator it( *fList );
      QFileInfo *fi;
      while ( (fi=it.current()) ) {
	QString name = fi->filePath();
	if(isDotLa) 
#ifdef WIN32
	  name.replace(QRegExp("\\.dll"), ".la");
#else
#ifdef MACOS_X
	  name.replace(QRegExp("\\.dylib"), ".la");
#else
	  name.replace(QRegExp("\\.so"), ".la");
#endif
#endif
	cout << name.latin1() << endl;
	ret.append(name);
	++it;
      }
  }

  void addResourceType(const QString& s, const QString& s2) {}
  KURL saveLocation(const QString& s) { return KURL(qApp->applicationDirPath()); }

};

#endif
