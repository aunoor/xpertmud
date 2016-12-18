#ifndef KPOPUPMENU_H
#define KPOPUPMENU_H

#include <QMenu>

class KPopupMenu: public QMenu {
  Q_OBJECT
 public:
  KPopupMenu(QObject *parent):
    QMenu(static_cast<QWidget *>(parent)) {}
};

#endif
