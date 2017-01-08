#ifndef XMUD_MAPPERBACKEND_H
#define XMUD_MAPPERBACKEND_H

#include <QObject>

class QWidget;

#define BACKEND_DUMMY 0

class XMMbackend:public QObject {
  Q_OBJECT
public:
  XMMbackend(QObject *parent = 0);
  ~XMMbackend() ;
  
public slots:
  void slotConfig(QWidget *parent);

}; 

#endif