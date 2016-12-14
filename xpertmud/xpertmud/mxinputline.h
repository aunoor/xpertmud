// -*- c++ -*-
/***************************************************************************
                          mxinputline.h  -  description
                             -------------------
    begin                : Tue Jun 19 2001
    copyright            : (C) 2001,2002 by Manuel Klimek & Ernst Bachmann
    email                : manue.klimek@web.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MXINPUTLINE_H
#define MXINPUTLINE_H

#include <qobject.h>

#include <qtextedit.h>
#include <vector>

class MXInputLine : public QTextEdit  {
  Q_OBJECT
public: 

  MXInputLine(const QString &string, QWidget *parent, const char *name = 0);

  virtual QSize sizeHint() const {
    return minimumSize();
  }

  virtual QSize minimumSizeHint() const {
    return minimumSize();
  }

  void setID(unsigned int id) {
    this->id = id;
  }

  int getID() const {
    return id;
  }

public slots:
  void slotEchoMode(bool);
  void slotCommandRetention(bool);
signals:
  void textEntered(const QString&);

private slots:
  void adjustHeight();

protected:
  void keyPressEvent(QKeyEvent *event);

private:

  std::vector<QString> buffer;
  // BufferPtr should always point to the currently displayed history entry.
  unsigned int bufferPtr;
  
  // last is only used to forbid multiple consequtive enties in buffer.
  QString last;

  // use command retention?
  bool command_retention;

  unsigned int id;
};

#endif
