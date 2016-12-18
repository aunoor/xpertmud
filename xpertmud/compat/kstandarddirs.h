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
    cout << "Searching for: " << type.toLatin1().data() << "/" << cfilter.toLatin1().data() << endl;
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
      const QFileInfoList fList = d.entryInfoList(QStringList() << filter);

      if(fList.isEmpty()) return ret;

      QListIterator<QFileInfo> it (fList);
      QFileInfo fi;

      while ( it.hasNext() ) {
          fi = it.next();
          QString name = fi.filePath();
          cout << name.toLocal8Bit().data() << endl;
          ret.append(name);
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
      if(filter.indexOf(".la") != -1) {
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
	cout << "Final Filter: " << filter.toLatin1().data() << endl;
      const QFileInfoList fList = d.entryInfoList(QStringList() << filter);

      if(fList.isEmpty()) return;

      QListIterator<QFileInfo> it (fList);
      QFileInfo fi;

      while (it.hasNext()) {
          fi = it.next();
	      QString name = fi.filePath();
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
          cout << name.toLatin1().data() << endl;
          ret.append(name);
      }
  }

  void addResourceType(const QString& s, const QString& s2) {}
  KURL saveLocation(const QString& s) { return KURL(qApp->applicationDirPath()); }

};

#endif
