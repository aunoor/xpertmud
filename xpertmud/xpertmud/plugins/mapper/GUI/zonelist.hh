#ifndef XMUD_MAPPERZONELIST_H
#define XMUD_MAPPERZONELIST_H

#include <qobject.h>
#include <qlistview.h>

class QWidget;


class XMMzonelist:public QListView {
  Q_OBJECT
public:
  XMMzonelist(QWidget *parent, const char *name);

public slots:
  void slotAddZone(QString zonename, int uniqueid);
};

#endif
