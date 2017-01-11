// -*- c++ -*-
#ifndef XMUD_MAPPERGUI_H
#define XMUD_MAPPERGUI_H

#include <QWidget>

class XMMAbstractBackend;
class XMMcore;
class XMMmenu;
class XMMmapView;
class XMMmap;
class XMMzonelist;
class QVBoxLayout;
class QSplitter;

class XmudMapper:public QWidget {
  Q_OBJECT
public:
  XmudMapper(QWidget *parent, const char *name, const QStringList & args);
  ~XmudMapper();
  XMMmenu *getMenuBar();

signals:
  void callback(int func, const QVariant & args, QVariant & result);
  void emitBackendSelected(XMMAbstractBackend *backend);
  void emitAddZone(QString zonename);
  void emitDelZone();
  void emitRenameZone(QString zonename);

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
  void slotAddTrigger();
  void slotDelTrigger();

protected:
  QSplitter *m_splitter;
  QVBoxLayout *m_layout;
  XMMmapView *m_mapview;
  XMMmenu *m_menu;
  XMMAbstractBackend *m_backend;
  XMMmap *m_map;
  XMMzonelist *m_zonelist;
};

#endif
