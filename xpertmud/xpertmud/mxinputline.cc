/***************************************************************************
                          mxinputline.cc  -  description
                             -------------------
    begin                : Tue Jun 19 2001
    copyright            : (C) 2001,2002 by Manuel Klimek & Ernst Bachmann
    email                : manuel.klimek@web.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mxinputline.h"
#include <QScrollBar>
#include <QKeyEvent>

MXInputLine::MXInputLine(const QString &/* unused: string*/, QWidget *parent, const char *name):
  QPlainTextEdit(parent),bufferPtr(0),
  command_retention(true), id(0)
{
  setObjectName(name);
  //setTextFormat(Qt::PlainText);
  setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
  //setWrapPolicy(AtWordOrDocumentBoundary);
  setMinimumWidth(150);
  adjustHeight();

  connect(this, SIGNAL(textChanged()),
	  this, SLOT(adjustHeight()));
}

void MXInputLine::keyPressEvent(QKeyEvent *event) {
  int para, pos;
  //getCursorPosition(&para, &pos);
  pos = textCursor().positionInBlock();
  para = textCursor().blockNumber();

  //x? setModified(false);

  //  QFontMetrics fm(font());
  // setFixedHeight(fm.height() * lines() + 10);

  if(event->modifiers() & Qt::ControlModifier) { // ctrl-return
    QPlainTextEdit::keyPressEvent(event);

  } else if((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {

    emit textEntered(toPlainText());
    if(toPlainText() != "" && toPlainText() != last) {
      last=toPlainText();
      buffer.push_back(last);
    }
    bufferPtr=buffer.size();
    if (command_retention) {
      moveCursor(QTextCursor::End);
      selectAll();
    } else {
      clear();
    }
    //x?setModified(true);
    
  } else if(event->key() == Qt::Key_Up && bufferPtr > 0 &&
	    para == 0) {

    /*
    cout << "KEY_UP! Text: [" << text().latin1() << "] last: [" << last.latin1() 
	 << "] bufferPtr: " << bufferPtr << " buffer.size(): " << buffer.size() << endl;
    */
    if(toPlainText() != "" && toPlainText() != last &&
       bufferPtr >= buffer.size()-1) {

      // This is the first "prev history" action,
      // the current text is unsent (otherwise it would be in last)
      // so store it now.
      last=toPlainText();
      buffer.push_back(last);
      bufferPtr= buffer.size()-1;
    }
    --bufferPtr;
    setPlainText(buffer[bufferPtr]);
    moveCursor(QTextCursor::End);

    //x?setModified(true);

  } else if(event->key() == Qt::Key_Down && bufferPtr+1 < buffer.size() &&
	    para == blockCount()-1) {

    ++bufferPtr;
    setPlainText(buffer[bufferPtr]);
    moveCursor(QTextCursor::End);
    //x?setModified(true);

  } else {
    QPlainTextEdit::keyPressEvent(event);
  }
}

void MXInputLine::adjustHeight() {
  int offset = 4;
  if(horizontalScrollBar()->isVisible()) {
    offset += horizontalScrollBar()->height();
  }
  setFixedHeight(qMin(200,heightForWidth(width())+offset));
}

void MXInputLine::slotEchoMode(bool mode) {
  // cout << "slotEchoMode " << (int)mode << endl;
  if(mode) {
    setEnabled(true);
    setFocus();
  } else {
    setPlainText("");
    setEnabled(false);
  }
}

void MXInputLine::slotCommandRetention(bool mode) {
  command_retention=mode;
}
