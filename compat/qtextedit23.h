#ifndef QTEXTEDIT_H
#define QTEXTEDIT_H

#if QT_VERSION < 300

#include <qmultilineedit.h>
#include <qscrollbar.h>

#ifdef WIN32
#pragma warning( disable : 4100 )
#endif

class QTextEdit: public QMultiLineEdit {
  Q_OBJECT
 public:
  QTextEdit(QWidget *parent, const char* name = 0):
    QMultiLineEdit(parent, name) {}

  QTextEdit(const QString& text, const QString& name, QWidget *parent):
    QMultiLineEdit(parent, name) {
    setText(text);
  }

  void setModified(bool s) { setEdited(s); }
  int paragraphs() { return 1; }
  void setTextFormat(int) {}
  int paragraphLength(int) { return text().length(); }
  virtual void setSelection(int para1, int char1,
			    int para2, int char2) {
    selectAll();
    /*
    int line = 0;
    int lastCount = 0;
    int count = lineLength(line)+1; // \n
    while(line < numLines() && char2 > count) {
      ++line;
      lastCount = count;
      count += lineLength(line)+1; // \n
    }
    QMultiLineEdit::setSelection(0, 0, 0, 2);*/
  }
  bool isModified() { return edited(); }
  
  virtual int heightForWidth(int width) {
    setFixedVisibleLines(numLines());
    int offset = 0;
    if(horizontalScrollBar()->isVisible()) {
      offset += horizontalScrollBar()->height();
    }
    QMultiLineEdit::setFixedHeight(height()+offset);
    return 0;
  }
  
  virtual void setFixedHeight(int height) {
  }
  
  
  virtual void keyPressEvent(QKeyEvent *event) {
    if(event->state() & Qt::ControlButton) { // ctrl-return
      QKeyEvent ev(QEvent::KeyPress, event->key(),
		   event->ascii(), 0);
      QMultiLineEdit::keyPressEvent(&ev);
    } else {
      QMultiLineEdit::keyPressEvent(event);
    }
  }
};

#else
#undef QTEXTEDIT_H
#include "qtextedit-original.h"
#endif
#endif
