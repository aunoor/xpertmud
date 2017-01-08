#ifndef XMUD_MAPPERZONELIST_H
#define XMUD_MAPPERZONELIST_H

#include <QListView>

class QWidget;


class XMMzonelist:public QListView {
  Q_OBJECT
public:
  XMMzonelist(QWidget *parent = 0);

public slots:
  void slotAddZone(QString zonename, int uniqueid);
};

#endif
