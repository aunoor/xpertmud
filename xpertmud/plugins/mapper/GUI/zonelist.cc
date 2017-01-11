#include "zonelist.hh"
#include <QTreeView>
#include <QVBoxLayout>

XMMzonelist::XMMzonelist(QWidget *parent):
  QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  m_zonesView = new QTreeView(this);
  layout->addWidget(m_zonesView);
}

XMMzonelist::~XMMzonelist() {

}

void XMMzonelist::slotAddZone(QString zonename, int uniqueid) {

}

void XMMzonelist::setModel(QAbstractItemModel *model) {
  m_zonesView->setModel(model);
}
