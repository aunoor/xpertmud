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

#include <qlabel.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qgrid.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qhgroupbox.h>
#include <qvgroupbox.h>
#include <qlistview.h>
#include <qpushbutton.h>

#include <kiconloader.h>
#include <klocale.h>

BookmarkEditor::BookmarkEditor(const Bookmark & original, QWidget * parent,
			       const char *name):
  KDialogBase(KDialogBase::IconList,i18n("Edit Bookmark"),
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


  commentsEdit=new QTextEdit(bookmark.getComments(),QString::null,page);
  commentsEdit->setTextFormat(PlainText);
  label->setBuddy(commentsEdit);

  page->setStretchFactor(commentsEdit,1);

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

  line->setStretchFactor(startInterp,1);

  QComboBox * scriptingSelector=new QComboBox(true,line);
  
  // TODO: read from avail Interpreters!!!
  scriptingSelector->insertItem("perl");
  scriptingSelector->insertItem("python");
  scriptingSelector->insertItem("ruby");
#if QT_VERSION < 300
  // TODO: add workaround
#else
  scriptingSelector->setCurrentText(bookmark.getInterpreter());
#endif
  scriptingSelector->setEnabled(startInterp->isChecked());

  connect(startInterp,SIGNAL(toggled(bool)),
	  scriptingSelector, SLOT(setEnabled(bool)));

  connect(scriptingSelector,SIGNAL(textChanged(const QString &)),
	  &bookmark,SLOT(setInterpreter(const QString &)));

  QCheckBox * resetInterp = new QCheckBox(i18n("Reset Interpreter if already running?"),
					  page);
  resetInterp->setChecked(bookmark.getResetInterpreter());
  resetInterp->setEnabled(startInterp->isChecked());

  connect(resetInterp,SIGNAL(toggled(bool)),
	  &bookmark,SLOT(setResetInterpreter(bool)));

  connect(startInterp,SIGNAL(toggled(bool)),
	  resetInterp, SLOT(setEnabled(bool)));

  QLabel * label=new QLabel(i18n("Script:"),page);

  globalScriptEdit=new QTextEdit(bookmark.getGlobalScript(),QString::null,page);
  globalScriptEdit->setTextFormat(PlainText);
  globalScriptEdit->setEnabled(startInterp->isChecked());
  label->setBuddy(globalScriptEdit);
  page->setStretchFactor(globalScriptEdit,1);

  connect(globalScriptEdit,SIGNAL(textChanged()),
	  this,SLOT(slotGlobalScriptChanged()));

  connect(startInterp,SIGNAL(toggled(bool)),
	  globalScriptEdit, SLOT(setEnabled(bool)));

  
}


void BookmarkEditor::addConnectionPage() {
  QVBox * page = addVBoxPage(i18n("Connections"),i18n("Connections:"),
			     BarIcon("connect_no",KIcon::SizeMedium) );
  
  QHGroupBox * conBox=new QHGroupBox(i18n("Connections:"),page);

  connectionList=new QListView(conBox);
  connectionList->addColumn(i18n("Id"));
  connectionList->addColumn(i18n("Host"),200);
  connectionList->addColumn(i18n("Port"));
  connectionList->addColumn(i18n("Encoding"));
  connectionList->setAllColumnsShowFocus(true);

  connect(connectionList,SIGNAL(selectionChanged(QListViewItem *)),
	  this,SLOT(slotSelectedConnectionChanged(QListViewItem *)));


  // conBox->setStretchFactor(connectionList,1);
  QVBox * buttonBox=new QVBox(conBox);
  addButton=new QPushButton(i18n("Add"),buttonBox);
  connect(addButton,SIGNAL(clicked()),
	  this,SLOT(slotAddConnection()));
  

  deleteButton=new QPushButton(i18n("Delete"),buttonBox);
  deleteButton->setEnabled(false);

  connect(deleteButton,SIGNAL(clicked()),
	  this,SLOT(slotDeleteConnection()));
  

  QVGroupBox * setBox=new QVGroupBox(i18n("Connection Settings:"),page);
  QGrid * line = new QGrid(2,QGrid::Vertical,setBox);
  QLabel * label;

  label=new QLabel(i18n("Id:"),line);
  idEdit=new QSpinBox(0,256,1,line);
  idEdit->setEnabled(false);
  connect(idEdit,SIGNAL(valueChanged(int)),
	  this, SLOT(slotIdChanged()));
  label->setBuddy(idEdit);

  label=new QLabel(i18n("Host:"),line);
  hostEdit=new QLineEdit(line);
  hostEdit->setEnabled(false);
  connect(hostEdit,SIGNAL(textChanged(const QString &)),
	  this, SLOT(slotHostChanged()));
  label->setBuddy(hostEdit);

  label=new QLabel(i18n("Port:"),line);
  portEdit=new QSpinBox(0,0xFFFF,1,line);
  portEdit->setEnabled(false);
  connect(portEdit,SIGNAL(valueChanged(int)),
	  this, SLOT(slotPortChanged()));
  label->setBuddy(portEdit);

  label=new QLabel(i18n("Login Sequence:"),setBox);
  loginEdit=new QTextEdit(setBox);
  loginEdit->setTextFormat(PlainText);
  loginEdit->setEnabled(false);
  connect(loginEdit, SIGNAL(textChanged()),
	  this, SLOT(slotScriptChanged()));
  label->setBuddy(loginEdit);

  QHBox * eBox=new QHBox(setBox);
  label=new QLabel(i18n("Encoding:"),eBox);
  encodingEdit=new QComboBox(true,eBox);
  encodingEdit->insertItem("<default>");
  encodingEdit->insertItem("ISO8859-1");
  encodingEdit->insertItem("UTF8");
  encodingEdit->insertItem("ISO8859-15");
  encodingEdit->insertItem("IBM850");
  encodingEdit->insertItem("CP1252");
  encodingEdit->setEnabled(false);
  encodingEdit->setDuplicatesEnabled(false);
  connect(encodingEdit, SIGNAL(textChanged(const QString &)),
	  this, SLOT(slotEncodingChanged(const QString &)));

  label->setBuddy(encodingEdit);
}

void BookmarkEditor::slotCommentsChanged() {
  // TODO: Check that comments doesn't contain ']]>'
  bookmark.setComments(commentsEdit->text());
}

void BookmarkEditor::slotGlobalScriptChanged() {
  // TODO: Check that script doesn't contain ']]>'
  bookmark.setGlobalScript(globalScriptEdit->text());
}

void BookmarkEditor::loadConnectionData() {
  connectionList->clear();
  QValueVector<int> connIDs = bookmark.getAvailableConnections();
  for (QValueVector<int>::iterator it=connIDs.begin();
       it!=connIDs.end();++it) {
    QString host=bookmark.getConnectionHost(*it);
    short unsigned int port=bookmark.getConnectionPort(*it);
    QString enc=bookmark.getConnectionEncoding(*it);
    (void)new QListViewItem(connectionList,
			    QString::number(*it),
			    host,
			    QString::number(port),
			    enc);
    
  }
}

void BookmarkEditor::slotAddConnection() {
  int id=0;
  while (bookmark.isConnectionAvailable(id)) 
    ++id;

  QString defaultHost;
  short unsigned int defaultPort=0;
  QString defaultEnc;
  QValueVector<int> connIDs = bookmark.getAvailableConnections();
  if (connIDs.size()) {
    defaultHost=bookmark.getConnectionHost(connIDs.front());
    defaultPort=bookmark.getConnectionPort(connIDs.front());
    defaultEnc=bookmark.getConnectionEncoding(connIDs.front());
  }
  
  bookmark.addConnection(id);
  bookmark.setConnectionHost(id,defaultHost);
  bookmark.setConnectionPort(id,defaultPort);
  bookmark.setConnectionEncoding(id,defaultEnc);

  QListViewItem *i=new QListViewItem(connectionList,
				     QString::number(id),
				     defaultHost,
				     QString::number(defaultPort),
				     defaultEnc);
  connectionList->setSelected(i,true);
}

void BookmarkEditor::slotDeleteConnection() {
  QListViewItem *i=connectionList->selectedItem();
  if (i==NULL) {
    // internal error, the delete button should've been disabled!!!
    return;
  }
  bool ok=false;
  int id=i->text(0).toInt(&ok);

  //remove in any case
  delete i;
  
  if (ok) {
    bookmark.removeConnection(id);
  }
  if (connectionList->selectedItem()==NULL) {
    currentConnection=-1;
    slotDisableConnectionEditors();
  }
}


void BookmarkEditor::slotSelectedConnectionChanged(QListViewItem * i) {
  if (i==NULL) {
    currentConnection=-1;
    slotDisableConnectionEditors();
  } else {
    bool ok=false;
    int id=i->text(0).toInt(&ok);
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
  loginEdit->setText(bookmark.getConnectionScript(currentConnection));

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
      QListViewItem * i = connectionList->selectedItem();
      if (i!=NULL) {
	i->setText(0,QString::number(currentConnection));
      } // else very strange
    }
  } // else very strange
}
void BookmarkEditor::slotHostChanged() {
  if (currentConnection>=0) {
    bookmark.setConnectionHost(currentConnection,hostEdit->text());

    QListViewItem * i = connectionList->selectedItem();
    if (i!=NULL) {
      i->setText(1,hostEdit->text());
    } // else very strange
  }  // else very strange
}
void BookmarkEditor::slotEncodingChanged(const QString & enc) {
  QString e=enc;
  if (e == "<default>")
      e="";
  if (currentConnection>=0) {
    bookmark.setConnectionEncoding(currentConnection,e);

    QListViewItem * i = connectionList->selectedItem();
    if (i!=NULL) {
      i->setText(3,e);
    } // else very strange
  }  // else very strange
}
void BookmarkEditor::slotPortChanged() {
  if (currentConnection>=0) {
    bookmark.setConnectionPort(currentConnection,portEdit->value());

    QListViewItem * i = connectionList->selectedItem();
    if (i!=NULL) {
      i->setText(2,QString::number(portEdit->value()));
    } // else very strange
  }  // else very strange
}
void BookmarkEditor::slotScriptChanged() {
  if (currentConnection>=0) {
    bookmark.setConnectionScript(currentConnection,loginEdit->text());
  }  // else very strange
}
