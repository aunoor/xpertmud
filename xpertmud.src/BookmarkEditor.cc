/***************************************************************************
   $Header: /cvsroot/xpertmud/xpertmud/xpertmud/xpertmud.h,v 1.70 2002/08/09 14:
                             -------------------
    begin                :  15.08.2002 17:00 MEST 2002
    copyright            : (C) 2001,2002 by Ernst Bachmann, Manuel Klimek

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "BookmarkEditor.h"

#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QTreeWidget>
#include <QPushButton>

#include <kiconloader.h>
#include <klocale.h>

BookmarkEditor::BookmarkEditor(const Bookmark & original, QWidget * parent,
			       const QString name):
  KDialogBase(i18n("Edit Bookmark"),
	      KDialogBase::Ok | KDialogBase::Help | KDialogBase::Cancel,
	      KDialogBase::Ok, parent,name), 
  bookmark(original),currentConnection(-1) {

  //  connect(&bookmark,SIGNAL(titleChanged(const QString &)),
  //	  this, SLOT(setCaption(const QString &)));

  //  setCaption(bookmark.getTitle());

  addGeneralPage();
  addScriptingPage();
  addConnectionPage();
  
  loadConnectionData();
}

void BookmarkEditor::addGeneralPage() {
  QVBox * page = addVBoxPage(i18n("General"),i18n("General Settings"),
			     BarIcon("contents", KIcon::SizeMedium) );
  
  QLabel * label;

  label=new QLabel(i18n("Name:"),page);

  QLineEdit * nameEdit =new QLineEdit(bookmark.getName(),page);

  label->setBuddy(nameEdit);

  connect(nameEdit,SIGNAL(textChanged(const QString &)),
	  &bookmark,SLOT(setName(const QString &)));

  label=new QLabel(i18n("Comments:"),page);


  commentsEdit=new QPlainTextEdit(bookmark.getComments(), page);
  label->setBuddy(commentsEdit);

  ////page->setStretchFactor(commentsEdit,1);

  connect(commentsEdit,SIGNAL(textChanged()),
	  this,SLOT(slotCommentsChanged()));


}

void BookmarkEditor::addScriptingPage() {
  QVBox * page = addVBoxPage(i18n("Scripting"),i18n("Scripting Settings:"),
			     BarIcon("pencil",KIcon::SizeMedium) );

  QHBox * line=new QHBox(page);

  QCheckBox * startInterp = new QCheckBox(i18n("Start specified Interpreter?"),
					  line);
  startInterp->setChecked(bookmark.getStartInterpreter());
  connect(startInterp,SIGNAL(toggled(bool)),
	  &bookmark,SLOT(setStartInterpreter(bool)));

  ///line->setStretchFactor(startInterp,1);

  QComboBox * scriptingSelector=new QComboBox(line);
  scriptingSelector->setEditable(true);
  
  // TODO: read from avail Interpreters!!!
  scriptingSelector->addItem("perl");
  scriptingSelector->addItem("python");
  scriptingSelector->addItem("ruby");
  scriptingSelector->setCurrentText(bookmark.getInterpreter());
  scriptingSelector->setEnabled(startInterp->isChecked());
  scriptingSelector->setEditable(false);

  connect(startInterp, &QCheckBox::toggled,
          [=](bool toggled){scriptingSelector->setEnabled(toggled);
              if (toggled) this->bookmark.setInterpreter(scriptingSelector->currentText());
          });

  connect(scriptingSelector,SIGNAL(currentTextChanged(const QString &)),
	  &bookmark,SLOT(setInterpreter(const QString &)));

  QCheckBox * resetInterp = new QCheckBox(i18n("Reset Interpreter if already running?"), page);
  resetInterp->setChecked(bookmark.getResetInterpreter());
  resetInterp->setEnabled(startInterp->isChecked());

  connect(resetInterp,SIGNAL(toggled(bool)),
	  &bookmark,SLOT(setResetInterpreter(bool)));

  connect(startInterp,SIGNAL(toggled(bool)),
	  resetInterp, SLOT(setEnabled(bool)));

  QLabel * label=new QLabel(i18n("Script:"),page);

  globalScriptEdit=new QPlainTextEdit(bookmark.getGlobalScript(), page);
  globalScriptEdit->setEnabled(startInterp->isChecked());
  label->setBuddy(globalScriptEdit);
  //page->setStretchFactor(globalScriptEdit,1);

  connect(globalScriptEdit,SIGNAL(textChanged()),
	  this,SLOT(slotGlobalScriptChanged()));

  connect(startInterp,SIGNAL(toggled(bool)),
	  globalScriptEdit, SLOT(setEnabled(bool)));
}


void BookmarkEditor::addConnectionPage() {
  QVBox * page = addVBoxPage(i18n("Connections"),i18n("Connections:"),
			     BarIcon("connect_no",KIcon::SizeMedium) );
  
  //QHGroupBox * conBox=new QHGroupBox(i18n("Connections:"),page);
  QGroupBox *conBox = new QGroupBox(i18n("Connections:"), page);
  QHBoxLayout *conLayout = new QHBoxLayout;
  conBox->setLayout(conLayout);

  connectionList=new QTreeWidget(conBox);
  conLayout->addWidget(connectionList);
  connectionList->setSelectionMode(QAbstractItemView::SingleSelection);
  connectionList->setSelectionBehavior(QAbstractItemView::SelectRows);
  connectionList->setHeaderLabels(QStringList() << i18n("Id")
                                                << i18n("Host")
                                                << i18n("Port")
                                                << i18n("Encoding"));
  connectionList->setColumnWidth(2, 200);
  //connectionList->addColumn(i18n("Id"));
  //connectionList->addColumn(i18n("Host"),200);
  //connectionList->addColumn(i18n("Port"));
  //connectionList->addColumn(i18n("Encoding"));
  connectionList->setAllColumnsShowFocus(true);

  connect(connectionList,SIGNAL(itemSelectionChanged()),
          this,SLOT(slotSelectedConnectionChanged()));


  // conBox->setStretchFactor(connectionList,1);
  QVBox * buttonBox=new QVBox(conBox);
  conLayout->addWidget(buttonBox);
  addButton=new QPushButton(i18n("Add"),buttonBox);
  connect(addButton,SIGNAL(clicked()),
	  this,SLOT(slotAddConnection()));
  

  deleteButton=new QPushButton(i18n("Delete"),buttonBox);
  deleteButton->setEnabled(false);

  connect(deleteButton,SIGNAL(clicked()),
	  this,SLOT(slotDeleteConnection()));
  

  QGroupBox * setBox=new QGroupBox(i18n("Connection Settings:"),page);
  setBox->setAlignment(Qt::Vertical);
  QVBoxLayout *setLayout = new QVBoxLayout;
  setBox->setLayout(setLayout);
    //QGrid * line = new QGrid(2,QGrid::Vertical,setBox);
  QWidget * line = new QWidget(setBox);
  setLayout->addWidget(line);

  QGridLayout *lineLayout = new QGridLayout();
  line->setLayout(lineLayout);

  QLabel * label;
// row 0
  label=new QLabel(i18n("Id:"),line);
  lineLayout->addWidget(label, 0, 0);

  idEdit=new QSpinBox(line);
  idEdit->setMinimum(0);
  idEdit->setMaximum(256);
  idEdit->setSingleStep(1);
  lineLayout->addWidget(idEdit, 0, 1);

  idEdit->setEnabled(false);
  connect(idEdit,SIGNAL(valueChanged(int)),
	  this, SLOT(slotIdChanged()));
  label->setBuddy(idEdit);

//row 1
  label=new QLabel(i18n("Host:"),line);
  lineLayout->addWidget(label, 1, 0);
  hostEdit=new QLineEdit(line);
  lineLayout->addWidget(hostEdit, 1, 1);
  hostEdit->setEnabled(false);
  connect(hostEdit,SIGNAL(textChanged(const QString &)),
	  this, SLOT(slotHostChanged()));
  label->setBuddy(hostEdit);

//row 2
  label=new QLabel(i18n("Port:"),line);
  lineLayout->addWidget(label, 2, 0);
  portEdit=new QSpinBox(line);
  portEdit->setMinimum(0);
  portEdit->setMaximum(0xffff);
  portEdit->setSingleStep(1);
  lineLayout->addWidget(portEdit, 2, 1);
  portEdit->setEnabled(false);
  connect(portEdit,SIGNAL(valueChanged(int)),
	  this, SLOT(slotPortChanged()));
  label->setBuddy(portEdit);

//row 3
  label=new QLabel(i18n("Login Sequence:"),setBox);
  lineLayout->addWidget(label, 3, 0);
  loginEdit=new QPlainTextEdit(setBox);
  lineLayout->addWidget(loginEdit, 3, 1);
  loginEdit->setEnabled(false);
  connect(loginEdit, SIGNAL(textChanged()),
	  this, SLOT(slotScriptChanged()));
  label->setBuddy(loginEdit);

//row4
  QHBox * eBox=new QHBox(setBox);
  lineLayout->addWidget(eBox, 4, 0);
  label=new QLabel(i18n("Encoding:"),eBox);
  encodingEdit=new QComboBox(eBox);
  //encodingEdit->setEditable(true);
  encodingEdit->addItem("<default>");
  encodingEdit->addItem("ISO8859-1");
  encodingEdit->addItem("UTF8");
  encodingEdit->addItem("ISO8859-15");
  encodingEdit->addItem("IBM850");
  encodingEdit->addItem("CP1252");
  encodingEdit->setEnabled(false);
  encodingEdit->setDuplicatesEnabled(false);
  connect(encodingEdit, SIGNAL(currentTextChanged(const QString &)),
	  this, SLOT(slotEncodingChanged(const QString &)));

  label->setBuddy(encodingEdit);
}

void BookmarkEditor::slotCommentsChanged() {
  // TODO: Check that comments doesn't contain ']]>'
  bookmark.setComments(commentsEdit->toPlainText());
}

void BookmarkEditor::slotGlobalScriptChanged() {
  // TODO: Check that script doesn't contain ']]>'
  bookmark.setGlobalScript(globalScriptEdit->toPlainText());
}

void BookmarkEditor::loadConnectionData() {
  connectionList->clear();
  QVector<int> connIDs = bookmark.getAvailableConnections();
  for (QVector<int>::iterator it=connIDs.begin();
       it!=connIDs.end();++it) {
    QString host=bookmark.getConnectionHost(*it);
    short unsigned int port=bookmark.getConnectionPort(*it);
    QString enc=bookmark.getConnectionEncoding(*it);
    QTreeWidgetItem *lw = new QTreeWidgetItem(connectionList, QStringList()
			                            << QString::number(*it)
			                            << host
			                            << QString::number(port)
			                            << enc);
    Q_UNUSED(lw);
  }
}

void BookmarkEditor::slotAddConnection() {
  int id=0;
  while (bookmark.isConnectionAvailable(id)) 
    ++id;

  QString defaultHost;
  short unsigned int defaultPort=0;
  QString defaultEnc;
  QVector<int> connIDs = bookmark.getAvailableConnections();
  if (connIDs.size()) {
    defaultHost=bookmark.getConnectionHost(connIDs.front());
    defaultPort=bookmark.getConnectionPort(connIDs.front());
    defaultEnc=bookmark.getConnectionEncoding(connIDs.front());
  }
  
  bookmark.addConnection(id);
  bookmark.setConnectionHost(id,defaultHost);
  bookmark.setConnectionPort(id,defaultPort);
  bookmark.setConnectionEncoding(id,defaultEnc);

  QTreeWidgetItem *i=new QTreeWidgetItem(connectionList, QStringList()
				     << QString::number(id)
				     << defaultHost
				     << QString::number(defaultPort)
				     << defaultEnc);
  connectionList->setItemSelected(i, true);
}

void BookmarkEditor::slotDeleteConnection() {
  QList<QTreeWidgetItem*> is=connectionList->selectedItems();
  if (is.isEmpty()) {
    // internal error, the delete button should've been disabled!!!
    return;
  }
  bool ok=false;
  int id=is.at(0)->text(0).toInt(&ok);

  if (ok) {
    bookmark.removeConnection(id);
  }
  if (connectionList->selectedItems().isEmpty()) {
    currentConnection=-1;
    slotDisableConnectionEditors();
  }
}


void BookmarkEditor::slotSelectedConnectionChanged() {
  if (connectionList->selectedItems().isEmpty()) {
    currentConnection=-1;
    slotDisableConnectionEditors();
  } else {
    QTreeWidgetItem *twi = connectionList->selectedItems().at(0);
    bool ok=false;
    int id=twi->data(0, Qt::DisplayRole).toString().toInt(&ok);
    if (ok) {
      currentConnection=id;
      slotFillConnectionEditors();
    } else {
      // very strange, shouldn't happen
      currentConnection=-1;
      slotDisableConnectionEditors();
    }
  }
}

void BookmarkEditor::slotFillConnectionEditors() {
  deleteButton->setEnabled(true);

  hostEdit->setEnabled(true);
  hostEdit->setText(bookmark.getConnectionHost(currentConnection));

  portEdit->setEnabled(true);
  portEdit->setValue(bookmark.getConnectionPort(currentConnection));

  idEdit->setEnabled(true);
  idEdit->setValue(currentConnection);

  loginEdit->setEnabled(true);
  loginEdit->setPlainText(bookmark.getConnectionScript(currentConnection));

  encodingEdit->setEnabled(true);
  QString e=bookmark.getConnectionEncoding(currentConnection);
  if (!e.length()) e="<default>";
  encodingEdit->setEditText(e);
}

void BookmarkEditor::slotDisableConnectionEditors() {
  // TODO: Clear edit fields??
  deleteButton->setEnabled(false);
  hostEdit->setEnabled(false);
  portEdit->setEnabled(false);
  idEdit->setEnabled(false);
  loginEdit->setEnabled(false);
  encodingEdit->setEnabled(false);
}

void BookmarkEditor::slotIdChanged() {
  if (currentConnection>=0) {
    int newID=idEdit->value();
    if (newID==currentConnection)
      return;
    if (bookmark.isConnectionAvailable(newID)) {
      // TODO: we need to swap something around
    } else {
      bookmark.renameConnection(currentConnection,newID);
      currentConnection=newID;
      QList<QTreeWidgetItem*> is = connectionList->selectedItems();
      if (!is.isEmpty()) {
	        is.at(0)->setText(0,QString::number(currentConnection));
      } // else very strange
    }
  } // else very strange
}
void BookmarkEditor::slotHostChanged() {
  if (currentConnection>=0) {
    bookmark.setConnectionHost(currentConnection,hostEdit->text());

    QList<QTreeWidgetItem*> is = connectionList->selectedItems();
    if (!is.isEmpty()) {
      is.at(0)->setText(1,hostEdit->text());
    } // else very strange
  }  // else very strange
}
void BookmarkEditor::slotEncodingChanged(const QString & enc) {
  QString e=enc;
  if (e == "<default>")
      e="";
  if (currentConnection>=0) {
    bookmark.setConnectionEncoding(currentConnection,e);

    QList<QTreeWidgetItem*> is = connectionList->selectedItems();
    if (!is.isEmpty()) {
      is.at(0)->setText(3,e);
    } // else very strange
  }  // else very strange
}
void BookmarkEditor::slotPortChanged() {
  if (currentConnection>=0) {
    bookmark.setConnectionPort(currentConnection,portEdit->value());

    QList<QTreeWidgetItem*> is = connectionList->selectedItems();
    if (!is.isEmpty()) {
      is.at(0)->setText(2,QString::number(portEdit->value()));
    } // else very strange
  }  // else very strange
}
void BookmarkEditor::slotScriptChanged() {
  if (currentConnection>=0) {
    bookmark.setConnectionScript(currentConnection,loginEdit->toPlainText());
  }  // else very strange
}
