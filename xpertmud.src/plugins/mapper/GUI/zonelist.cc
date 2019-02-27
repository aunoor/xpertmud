#include "zonelist.hh"
#include <qwidget.h>
#include <qstring.h>

XMMzonelist::XMMzonelist(QWidget *parent = 0, const char *name = 0):
  QListView(parent, name) {
  int sortcol;

  sortcol = addColumn("Zone Name");
  addColumn("Unique Id");
  setSorting(sortcol, true);
  this->show();
}


void XMMzonelist::slotAddZone(QString zonename, int uniqueid) {
  QListViewItem *item;
  QString idlabel = "%1";
  item = new QListViewItem(this, zonename, idlabel.arg(uniqueid));
  CHECK_PTR(item);
  insertItem(item);
}
