#ifndef KFILEDIALOG_H
#define KFILEDIALOG_H

#include <qfiledialog.h>
#include "kurl.h"
#include <qapplication.h>

class KFileDialog: public QFileDialog {
  Q_OBJECT
 public:
  static KURL getOpenURL(KURL initially = QString::null, 
		     const QString & filter = QString::null, 
		     QWidget * parent = 0, const char * name = 0) {
    if (initially.isEmpty()) {
       initially=qApp->applicationDirPath();
    }
    return getOpenFileName(initially.url(), filter, parent, name);
  }
  static KURL getSaveURL(KURL  initially = QString::null, 
		     const QString & filter = QString::null, 
		     QWidget * parent = 0, const char * name = 0) {
    if (initially.isEmpty()) {
       initially=qApp->applicationDirPath();
    }
    return getSaveFileName(initially.url(), filter, parent, name);
  }
};

#endif
