// -*- c++ -*-
#ifndef TEXTBUFFERVIEW_H
#define TEXTBUFFERVIEW_H


#include <qwidget.h>
#include <qfont.h>
#include <qsize.h>
#include <qdatetime.h>
#include <qpixmap.h>
#include <map>
#include "ColorChar.h"
#include "TextBufferViewIface.h"

class TextBuffer;

/**
 * A widget that shows a TextBuffer.
 */
class TextBufferView: public QWidget, public TextBufferViewIface {
  Q_OBJECT
public:
  enum EFollowMode {
    EFM_DontFollow,
    EFM_AlwaysFollow,
    EFM_LastLineFollow
  };

  TextBufferView(int id, QWidget* parent, const char* name,
		 const QColor* cmap, const QFont& font,
		 TextBuffer* cTextBuffer = NULL,
		 bool fixBuffer = true);
  virtual ~TextBufferView();

  TextBuffer* buffer() { return textBuffer; }
  void switchMouseMode(bool enabled);
  void setScriptingFont(const QFont& newFont);
  QSize sizeForChars(unsigned int x, unsigned int y);
  void scrollLines(int delta);

  void setFollowMode(EFollowMode mode) {
    followMode = mode;
  }
  unsigned int getLines() { return lines; }
  unsigned int getColumns() { return columns; }
  void setOffsetX(int offset) { offsetX = offset; update(); }
  void setOffsetY(int offset) { offsetY = offset; update(); }
  int getOffsetX() { return offsetX; }
  int getOffsetY() { return offsetY; }
  unsigned int getLineHeight() { return fontH; }
  int getID() { return winID; }
  //  virtual void setGeometry( int x, int y, int w, int h );

public slots:
  void slotTextChanged(int column, int line,
		       unsigned int width, unsigned int height);
  void slotTextChanged();
  void slotScrolledLines(int);
  void slotScrolledColumns(int);
  void slotFrontCut(int);

  void slotColorConfigChanged();
  void slotClearSelection();
  void slotClearWindow();
  void slotChangeFont();
  void slotNewDefaultFont(const QFont & newFont);
  void slotCopySelectedContents();

signals:
  void scriptingMousePressEvent(int id, int x, int y);
  void scriptingMouseDragEvent(int id, int x, int y);
  void scriptingMouseReleaseEvent(int id, int x, int y);
  void offsetYChange(int);

protected:
  void connectTextBuffer();

  void calcGeometry();
  void fontChange(const QFont& newFont);

  void closeEvent(QCloseEvent* ce);
  void paintEvent(QPaintEvent* pe);
  void resizeEvent(QResizeEvent* re);

  // update chars by finding longest strings with equal
  // attributes
  //  void updateCharsUni(int x,int y,int w,int h, bool redraw=true);
  //  void drawAttrStr(QPainter* paint, const QRect& rect,
  //		   QString text, ColorChar attr);

  // update chars step-by-step, which solves a lot of
  // screen garbage problems and can make use of
  // caching per char
  void updateChars(int lux, int luy, int w, int h);
  void drawChars(QPaintDevice* pd, int x,int y,int w,int h);
  void drawChar(QPaintDevice* pd, int x, int y);

  /**
   * Calculates the new y-offset on scroll and cut events.
   */
  void recalcOffsetY(int delta, bool cut);

  void mousePressEvent(QMouseEvent* ev);
  void mouseMoveEvent(QMouseEvent* ev);
  void mouseReleaseEvent(QMouseEvent* ev);
  void mouseDoubleClickEvent(QMouseEvent * ev);


  /*
    Offset (in pixel) from the left to the  text.
    Doesn't work for upper, right and lower
    borders because they usually have parts of
    the text visible.
  */
  int leftBorderWidth;

  TextBuffer* textBuffer;
  /*
    ++++++++++++++++++++++++++++++++++++++
    +  TextBuffer                        +
    +                                    +
    +                                    +
    +                                    +
    +                                    +
    +    (offsetX, offsetY)              +
    +           ---------------------    +
    +          | TextBufferView      |   +
    +          |                     | lines
    +          |                     |   +
    +          |                     |   +
    +           ---------------------    +
    ++++++++++++++++++ columns +++++++++++    

    - offsetX, offsetY may be < 0
    - the right lower point may be out of the textBuffer, too
   */
  int offsetX;
  int offsetY;
  unsigned int columns;
  unsigned int lines;

  // if set, the TextBuffer's ansi pane always has
  // the same size as this View.
  // if textBuffer->growBuffer == false, the textBuffer's buffer
  //    is always the same size as it's ansi pane
  bool fixTextBuffer;


  // the internal window identifier
  const int winID;

  bool blinking;

  bool isDefaultFont;
  int fontH, fontW, fontA; // height/width/ascend

  const QColor* colorTable;

  QClipboard* clipboard;
  bool selectingEnabled;
  bool selecting;
  bool colorSelectingMux;

  // where the starting clic has been made
  int selBeginX, selBeginY;

  // start of selected area
  int selStartX, selStartY;

  // end of selected area
  int selEndX, selEndY;

  // character pos of the last scripting movement notification
  int scrLastX, scrLastY;

  // true if scripting has received mouseDown and should get DRAG notifications
  bool scriptingDrag;

  // if we created the textBuffer, we'll have to delete it...
  bool ownsBuffer;

  // for nice looking drawing...
  QPixmap* drawBuffer;

  // cache: Color char -> int #nr in charCachePixmap
  // the cache will be at position fontW * #nr
  typedef std::map<ColorChar, int> charCacheT;
  QPixmap charCachePixmap;
  charCacheT charCache;
  unsigned int nextCachePoint;
  int maxCache;

  EFollowMode followMode;
};

#endif
