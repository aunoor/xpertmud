// -*- c++ -*-
#ifndef BATTLE_CONTACT_WIDGET_H
#define BATTLE_CONTACT_WIDGET_H

#include <qwidget.h>
#include <qvariant.h>
#include <map>

class BattleCore;
class QListView;
class QListViewItem;
class MechInfo;
class QPopupMenu;
class ContactItem;

class BattleContactWidget: public QWidget {
  Q_OBJECT
public:
  BattleContactWidget(QWidget *parent, const char *name, const QStringList& args);
  ~BattleContactWidget();

  // <Plugin Interface> ///////////////////////////////////////////////////////
signals:
  void callback(int func, const QVariant & args, QVariant & result);

public slots:
  void slotFunctionCall(int func, const QVariant & args, QVariant & result);
  // </Plugin Interface> //////////////////////////////////////////////////////

public slots:
  void popmeup(QListViewItem *, const QPoint &, int);  

protected slots:
  void slotUpdateMechInfo(const MechInfo &,const MechInfo &);
//  void updateList();
  void slotPopup(int);

private:
  BattleCore * core;
  QListView* listView;
  QPopupMenu * popup;
  QString currentID;
  std::map<QString, ContactItem*> items;
};

#endif
