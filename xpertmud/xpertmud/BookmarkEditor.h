// -*- c++ -*-
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
#ifndef BOOKMARK_EDITOR_H
#define BOOKMARK_EDITOR_H
#include <kdialogbase.h>

#include "Bookmark.h"

class QTextEdit;
class QListView;
class QListViewItem;
class QLineEdit;
class QSpinBox;
class QComboBox;
class QPushButton;
class QComboBox;

class BookmarkEditor: public KDialogBase {
  Q_OBJECT
public:
  BookmarkEditor(const Bookmark & original, QWidget * parent=0,const char *name=0);
  
  const Bookmark & getBookmark() const { return bookmark; }

protected:
  void addGeneralPage();
  void addScriptingPage();
  void addConnectionPage();

  void loadConnectionData();
  
protected slots:
  void slotCommentsChanged();
  void slotGlobalScriptChanged();
  void slotAddConnection();
  void slotDeleteConnection();
  void slotSelectedConnectionChanged(QListViewItem *);

  void slotFillConnectionEditors();
  void slotDisableConnectionEditors();
  
  void slotIdChanged();
  void slotHostChanged();
  void slotPortChanged();
  void slotScriptChanged();
  void slotEncodingChanged(const QString &);
protected:
  Bookmark bookmark;
  QTextEdit * commentsEdit;
  QTextEdit * globalScriptEdit;
  QListView * connectionList;
  QLineEdit * hostEdit;
  QSpinBox  * idEdit;
  QSpinBox  * portEdit;
  QComboBox * encodingEdit;
  QTextEdit * loginEdit;
  QPushButton * addButton;
  QPushButton * deleteButton;
  int currentConnection;
};

#endif
