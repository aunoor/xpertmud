// -*- c++ -*-
#ifndef XMUD_EXAMPLE_H
#define XMUD_EXAMPLE_H

#include <qwidget.h>
#include <qvariant.h>
class QButton;
class QLabel;

class XmudExample:public QWidget {
  Q_OBJECT
public:
  XmudExample(QWidget * parent, const char * name,const QStringList & args);

  // <Plugin Interface>
signals:
  void callback(int func, const QVariant & args, QVariant & result);

public slots:
  void slotFunctionCall(int func, const QVariant & args, QVariant & result);
  // </Plugin Interface>

protected slots:
  void slotButtonDown(); 
protected:
  QButton * myButton;
  QLabel * myLabel;
};

#endif
