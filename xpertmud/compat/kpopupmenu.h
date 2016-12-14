#ifndef KPOPUPMENU_H
#define KPOPUPMENU_H

#include <qpopupmenu.h>

class KPopupMenu: public QPopupMenu {
  Q_OBJECT
 public:
  KPopupMenu(QObject *parent):
    QPopupMenu(static_cast<QWidget *>(parent)) {}

};

#endif
