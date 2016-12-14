// -*- c++ -*-
#ifndef BATTLETECH_CORE_WIDGET_H
#define BATTLETECH_CORE_WIDGET_H
// This is a Widget used as Perl/Python representation of the BattleCore data


#include <qwidget.h>
#include <qvariant.h>

class BattleCore;
class QTextEdit;
class MechInfo;

class BattleCoreWidget: public QWidget {
  Q_OBJECT
public:
  BattleCoreWidget(QWidget * parent, const char * name,const QStringList & args);
  ~BattleCoreWidget();


  // <Plugin Interface> ///////////////////////////////////////////////////////
signals:
  void callback(int func, const QVariant & args, QVariant & result);

public slots:
  void slotFunctionCall(int func, const QVariant & args, QVariant & result);
  // </Plugin Interface> //////////////////////////////////////////////////////

protected slots:
  // puts one line of log in the LogWidget
  void slotLog(const QString &);
  
  // output one line to hud
  void slotSend(const QString &);

  // mechinfo change for scripting callback
  void mechInfoChange(const MechInfo& oldInfo, const MechInfo& mechInfo);
private:
  BattleCore * core;
  QTextEdit * logViewer;
};

#endif
