// -*- c++ -*-
#ifndef BATTLE_CONTACT_WIDGET_H
#define BATTLE_CONTACT_WIDGET_H

#include <QWidget>
#include <QVariant>
#include <QAbstractItemModel>
#include <map>

class BattleCore;
class QTreeView;
class MechInfo;
class QMenu;
class ContactItem;

class ContactsModel;

class QTreeViewItem: public QObject {

};

class ContactsModel: public QAbstractItemModel {

Q_OBJECT

public:
    ContactsModel(QObject *parent);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void updateMechInfo(const MechInfo &oldInfo, const MechInfo &mechInfo);

private:
    BattleCore * core;
    QVector<MechInfo> m_contacts;
    int getIndex(const MechInfo &info);
};

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
  //void popmeup(QTreeViewItem *, const QPoint &, int);
  void popmeup(const QPoint &point);

protected slots:
  void slotUpdateMechInfo(const MechInfo &,const MechInfo &);
//  void updateList();
  void slotPopup();

private:
  BattleCore * core;
  QTreeView* listView;
  QMenu * popup;
  QString currentID;
  std::map<QString, ContactItem*> items;
  ContactsModel *con_model;
};

#endif
