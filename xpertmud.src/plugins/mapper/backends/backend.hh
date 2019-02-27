#ifndef XMUD_MAPPERBACKEND_H
#define XMUD_MAPPERBACKEND_H
#include <qobject.h>
class QWidget;

#define BACKEND_DUMMY 0

class XMMbackend:public QObject {
  Q_OBJECT
public:
  XMMbackend(QObject *parent, const char *name);
  ~XMMbackend() ;
  
public slots:
  void slotConfig(QWidget *parent);

}; 

#endif

