#ifndef KICONLOADER_H
#define KICONLOADER_H

#include <qiconset.h>
#include <qstring.h>

#include "kglobal.h"

#ifdef WIN32
#pragma warning( disable : 4100 )
#endif

class KIcon {
 public:
  enum ESize {
    SizeMedium,
    Small
  };
};

class BarIcon {
 public:
  BarIcon(const QString& name, KIcon::ESize size) {}
};

class SmallIcon: public QIconSet {
 public:
  SmallIcon(const QString& name) {
  }
};

class KIconLoader {
 public:
  QPixmap loadIcon(const QString& name, KIcon::ESize size) {
    QPixmap ret;

    QString fileName;
    if(name == "appIcon" && size == KIcon::Small) {
      fileName = "hi16-app-xpertmud";
    }

    QStringList list = 
      KGlobal::dirs()->findAllResources("icon", fileName + ".png", 
					false, true);
    if(list.count() == 0) {
      return ret;
    }
    ret.load(list.first(), "PNG");
    return ret;
  }
};

#endif
