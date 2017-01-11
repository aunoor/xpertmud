#ifndef XMUD_MAPPERBACKEND_H
#define XMUD_MAPPERBACKEND_H

#include "core_global.h"
#include <QObject>

class QWidget;
class XmudMapper;
class XMObject;

class XMMAbstractBackend:public QObject {
  Q_OBJECT
public:
  XMMAbstractBackend(QObject *parent = 0);
  ~XMMAbstractBackend() ;

  virtual QString getBackendName() = 0;
  virtual XMBackends getId() = 0;

  virtual void parseLine(QString line);

protected:
  virtual void configShow(QWidget *parent);
  XmudMapper* getParent();

private:
  XmudMapper* m_parent;

public slots:
  void slotConfig(QWidget *parent);

signals:
  void newRoomSignal(XMObject *object);

}; 

#endif