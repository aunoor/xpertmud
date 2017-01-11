#ifndef XMUD_MAPPERZONELIST_H
#define XMUD_MAPPERZONELIST_H

#include <QWidget>
#include <QtCore/QAbstractItemModel>

class QTreeView;

class XMMzonelist:public QWidget {
  Q_OBJECT
public:
  XMMzonelist(QWidget *parent = 0);
  virtual ~XMMzonelist();

  void setModel(QAbstractItemModel *model);

private:
  QTreeView *m_zonesView;

public slots:
  void slotAddZone(QString zonename, int uniqueid);



};

#endif
