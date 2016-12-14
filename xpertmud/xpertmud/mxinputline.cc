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
#include <qscrollbar.h>

MXInputLine::MXInputLine(const QString &/* unused: string*/, QWidget *parent, const char *name):
  QTextEdit(parent, name),bufferPtr(0),
  command_retention(true), id(0)
{
  setTextFormat(Qt::PlainText);
  setWrapPolicy(AtWordOrDocumentBoundary);
  setMinimumWidth(150);
  adjustHeight();

  connect(this, SIGNAL(textChanged()),
	  this, SLOT(adjustHeight()));
}

void MXInputLine::keyPressEvent(QKeyEvent *event) {
  int para, pos;
  getCursorPosition(&para, &pos);

  //x? setModified(false);

  //  QFontMetrics fm(font());
  // setFixedHeight(fm.height() * lines() + 10);

  if(event->state() & Qt::ControlButton) { // ctrl-return
    QTextEdit::keyPressEvent(event);

  } else if((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {

    emit textEntered(text());
    if(text() != "" && text() != last) {
      last=text();
      buffer.push_back(last);
    }
    bufferPtr=buffer.size();
    if (command_retention) {
#if QT_VERSION<300
      setCursorPosition(numCols(),numRows());
#else
      moveCursor(MoveEnd,false);
#endif
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
    if(text() != "" && text() != last && 
       bufferPtr >= buffer.size()-1) {

      // This is the first "prev history" action,
      // the current text is unsent (otherwise it would be in last)
      // so store it now.
      last=text();
      buffer.push_back(last);
      bufferPtr= buffer.size()-1;
    }
    --bufferPtr;
    setText(buffer[bufferPtr]);
#if QT_VERSION<300
    setCursorPosition(numCols(),numRows());
#else
    moveCursor(MoveEnd,false);
#endif

    //x?setModified(true);

  } else if(event->key() == Qt::Key_Down && bufferPtr+1 < buffer.size() &&
	    para == paragraphs()-1) {

    ++bufferPtr;
    setText(buffer[bufferPtr]);
#if QT_VERSION<300
    setCursorPosition(numCols(),numRows());
#else
    moveCursor(MoveEnd,false);
#endif
    //x?setModified(true);

  } else {
    QTextEdit::keyPressEvent(event);
  }
}

void MXInputLine::adjustHeight() {
  int offset = 4;
  if(horizontalScrollBar()->isVisible()) {
    offset += horizontalScrollBar()->height();
  }
  setFixedHeight(QMIN(200,heightForWidth(width())+offset));
}

void MXInputLine::slotEchoMode(bool mode) {
  // cout << "slotEchoMode " << (int)mode << endl;
  if(mode) {
    setEnabled(true);
    setFocus();
  } else {
    setText("");
    setEnabled(false);
  }
}

void MXInputLine::slotCommandRetention(bool mode) {
  command_retention=mode;
}
