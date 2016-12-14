// -*- c++ -*-
#ifndef TEXTBUFFERHISTORYVIEW_H
#define TEXTBUFFERHISTORYVIEW_H

#include <qwidget.h>

#include "TextBufferViewIface.h"

class TextBufferView;
class TextBuffer;
class QScrollBar;
class QFrame;

/**
 * A widget that shows multiple views on
 * a TextBuffer (e.g. Splitscreen) which are
 * scrollable.
 */
class TextBufferHistoryView: public QWidget, public TextBufferViewIface {
  Q_OBJECT
public:
  TextBufferHistoryView(int winID, QWidget* parent, const char* name, 
			const QColor* cmap,
			const QFont& font, unsigned int scrollBackLines=7);
  virtual ~TextBufferHistoryView();

  TextBuffer* buffer();
  void switchMouseMode(bool enabled);
  void setScriptingFont(const QFont& newFont);
  QSize sizeForChars(unsigned int x, unsigned int y);
  void scrollLines(int delta);

  int getID();

public slots:
  void slotScrolledLines(int);
  void slotScriptingMousePressEvent(int id, int x, int y);
  void slotScriptingMouseDragEvent(int id, int x, int y);
  void slotScriptingMouseReleaseEvent(int id, int x, int y);
  void slotColorConfigChanged();
  void slotNewDefaultFont(const QFont & newFont);


protected slots:
  void slotVScroll(int);
  void slotMainOffsetYChange(int);

signals:
  void scriptingMousePressEvent(int id, int x, int y);
  void scriptingMouseDragEvent(int id, int x, int y);
  void scriptingMouseReleaseEvent(int id, int x, int y);

protected:
  bool eventFilter( QObject *o, QEvent *e );
  void wheelEvent(QWheelEvent* ev);
  void mousePressEvent(QMouseEvent* ev);
  void mouseMoveEvent(QMouseEvent* ev);
  void mouseReleaseEvent(QMouseEvent* ev);
  void resizeEvent(QResizeEvent*);
  void paintEvent(QPaintEvent*);
  void updateLayout();
  void updateVScrollBar();

  TextBuffer* textBuffer;
  TextBufferView* mainView;
  TextBufferView* splitView;
  QFrame* splitSeperator;

  bool inUpdate;
  QScrollBar* vscrollBar;

  bool ownsBuffer;
  int winID;

  bool scrollSplitEnabled;
  unsigned int scrollBackLines;
  bool movingSplit;
};

inline TextBuffer* TextBufferHistoryView::buffer() {
  return textBuffer;
}

inline int TextBufferHistoryView::getID() {
  return winID;
}

#endif
