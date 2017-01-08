#include "zonelist.hh"

XMMzonelist::XMMzonelist(QWidget *parent):
  QListView(parent) {
  int sortcol;
/*
  sortcol = addColumn("Zone Name");
  addColumn("Unique Id");
  setSorting(sortcol, true);
*/
  this->show();
}


void XMMzonelist::slotAddZone(QString zonename, int uniqueid) {
/*
  QListViewItem *item;
  QString idlabel = "%1";
  item = new QListViewItem(this, zonename, idlabel.arg(uniqueid));
  insertItem(item);
*/
}
