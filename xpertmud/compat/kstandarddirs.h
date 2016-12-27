#ifndef KSTANDARDDIRS_H
#define KSTANDARDDIRS_H

#ifdef WIN32
#pragma warning( disable : 4100 )
#endif

#include "kurl.h"
#include <QStringList>
#include <QDir>
#include <QRegExp>
#include <QApplication>
#include <QDebug>
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

  QStringList findAllResources(const QString& type, const QString& cfilter, bool, bool);

  /*Need keep implementation there for script plugins working */
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

  void findModules(const QString& dir, const QString &cfilter, QStringList& ret);

  void addResourceType(const QString& s, const QString& s2) {}
  KURL saveLocation(const QString& s) { return KURL(qApp->applicationDirPath()); }

};

#endif
