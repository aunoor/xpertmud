// -*- c++ -*-
#ifndef XMUD_MAPPERGUI_H
#define XMUD_MAPPERGUI_H
#include <qwidget.h>
#include <qvariant.h>
class XMMbackend;
class XMMcore;
class XMMmenu;
class XMMmapView;
class XMMmap;
class QHBoxLayout;
class XMMzonelist;

class XmudMapper:public QWidget {
  Q_OBJECT
public:
  XmudMapper(QWidget *parent, const char *name, const QStringList & args);
  ~XmudMapper();
  
signals:
  void callback(int func, const QVariant & args, QVariant & result);
  void emitBackendSelected(XMMbackend *backend);
  void emitAddZone(QString *zonename);

public slots:
  void slotFunctionCall(int func, const QVariant & args, QVariant & result);
  void slotSelectBackend(int backend_id);
  void slotNewMap();
  void slotOpenMap();
  void slotCloseMap();
  void slotSaveMap();
  void slotNewZoneDialog();
  void slotChangeZone(int zoneid);
  void slotRenameZoneDialog();
  void slotDelZoneDialog();

protected:
  QHBoxLayout *layout;
  XMMmapView *mapview;
  XMMmenu *menu;
  XMMbackend *backend;
  XMMmap *map;
  XMMzonelist *zonelist;
};

#endif
