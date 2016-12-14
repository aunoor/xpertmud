// -*- c++ -*-
#ifndef XMUD_MAPPERGUI_H
#define XMUD_MAPPERGUI_H

#include <qwidget.h>
#include <qvariant.h>
class QListBox;
class MapperCanvas;
class QCanvasView;
class QMenuBar;

class XmudMapper:public QWidget {
  Q_OBJECT
public:
  XmudMapper(QWidget *parent, const char *name, const QStringList & args);

  
signals:
  void callback(int func, const QVariant & args, QVariant & result);
  
public slots:
  void slotFunctionCall(int func, const QVariant & args, QVariant & result);


protected:
  QListBox *zonelist;
  MapperCanvas *mapcanvas;
  QCanvasView *mapview;
  QMenuBar *menu;
};

#endif
