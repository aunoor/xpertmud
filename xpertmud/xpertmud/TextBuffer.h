// -*- c++ -*-
#ifndef TEXTBUFFER_H
#define TEXTBUFFER_H

#include <QObject>
#include <QString>
#include <vector>
#include <deque>
#include <assert.h>

// cut this when unicode enabled
#include <string>


#include "ColorChar.h"
#include "CtrlParser.h"

#ifdef min
# undef min
#endif

class TextBuffer: public QObject, public CtrlCallbackIface {
  Q_OBJECT
public:
  TextBuffer(unsigned int initialWidth, 
	     unsigned int initialHeight,
	     unsigned int ansiViewWidth,
	     unsigned int ansiViewHeight,
	     int wordWrapColumn);
  
  TextBuffer(unsigned int initialWidth, 
	     unsigned int initialHeight,
	     bool growBuffer);
  
  TextBuffer(unsigned int initialWidth, 
	     unsigned int initialHeight,
	     int wordWrapColumn,
	     bool growBuffer);
  
  virtual ~TextBuffer() {}

  /**
   * Every single QChar in the text will be drawn
   * into the buffer as a character. Nothing will
   * be interpreted, use print() for interpretation.
   */
  virtual void printRaw(const QString& text);

  /**
   * The text will be interpreted as text with
   * ECMA control characters (newline, beep,
   * ansi codes, etc)
   */
  virtual void print(const QString& text);

  virtual void setBGColor(int c) { currentAttributes.setBg(c); }
  virtual void setFGColor(int c) { currentAttributes.setFg(c); }
  virtual void resetBGColor() {
    currentAttributes.setAttribute(ColorChar::A_DEFAULTBG);
  }
  virtual void resetFGColor() {
    currentAttributes.setAttribute(ColorChar::A_DEFAULTFG);
  }
  virtual void resetAttributes() { currentAttributes = defaultAttributes; }
  virtual void setIntense(bool to) { currentAttributes.setAttribute(ColorChar::A_INTENSE, to); }
  virtual void setFaint(bool to) { currentAttributes.setAttribute(ColorChar::A_FAINT, to); }
  virtual void setItalic(bool to) { currentAttributes.setAttribute(ColorChar::A_ITALIC, to); }
  virtual void setUnderline(bool to) { currentAttributes.setAttribute(ColorChar::A_UNDERLINE, to); }
  virtual void setBlinking(bool to) { currentAttributes.setAttribute(ColorChar::A_BLINK, to); }
  virtual void setRapidBlinking(bool to) { currentAttributes.setAttribute(ColorChar::A_RAPIDBLINK, to); }
  virtual void setReverse(bool to) { currentAttributes.setAttribute(ColorChar::A_REVERSE, to); }
  virtual void setConcealed(bool to) { currentAttributes.setAttribute(ColorChar::A_CONCEALED, to); }
  virtual void setStrikeout(bool to) { currentAttributes.setAttribute(ColorChar::A_STRIKEOUT, to); }
  virtual void setFraktur(bool to) { currentAttributes.setAttribute(ColorChar::A_FRAKTUR, to); }
  virtual void setDoubleUnderline(bool to) { currentAttributes.setAttribute(ColorChar::A_DUNDERLINE, to); }
  virtual void setFramed(bool to) { currentAttributes.setAttribute(ColorChar::A_FRAMED, to); }
  virtual void setEncircled(bool to) { currentAttributes.setAttribute(ColorChar::A_ENCIRCLED, to); }
  virtual void setOverline(bool to) { currentAttributes.setAttribute(ColorChar::A_OVERLINE, to); }
  virtual void setBold(bool to) { currentAttributes.setAttribute(ColorChar::A_BOLD, to); }
  virtual void clearEOL();
  virtual void clearBOL();
  virtual void clearLine();
  virtual void clearEOS();
  virtual void clearBOS();
  virtual void clearWindow();
  virtual void setCursor(unsigned int x, unsigned int y) {
    cursorX = std::min(ansiViewWidth-1, x);
    cursorY = std::min(ansiViewHeight-1, y);
  }
  virtual unsigned int getCursorX() { return cursorX; }
  virtual unsigned int getCursorY() { return cursorY; }
  virtual void newline();
  virtual void tab();
  virtual void backspace();
  virtual void beep();
  virtual void carriageReturn();

  virtual void scrollLines(int);
  virtual void scrollColumns(int);

  QChar getCharAt(unsigned int x, unsigned int y) {
    if(y < buffer.size() && x < buffer[y].size())
      return buffer[y][x].getChar();
    return QChar();
  }

  ColorChar getColorCharAt(unsigned int x, unsigned int y) {
    if(y < buffer.size() && x < buffer[y].size())
      return buffer[y][x];
    return ColorChar();
  }

  virtual unsigned int getLines() { return ansiViewHeight; }
  virtual unsigned int getColumns() { return ansiViewWidth; }

  void setWordWrapColumn(int c) {
    if(autoFeedForward) wordWrapColumn = c;
  }
  int getWordWrapColumn() {
    if(autoFeedForward)
      return wordWrapColumn;
    return 0;
  }

  void setDefaultFGColor(int color);
  void setDefaultBGColor(int color);

  /**
   * Resize the ansi view pane.
   * If !growBuffer, the whole buffer is resized, too.
   */
  void resize(unsigned int width, unsigned int height);

  ColorChar getBufferChar(int column, int line);
  unsigned int getSizeY();
  bool isGrowBuffer();

public slots:
  void setMaxBufferSize(unsigned int size) {
    maxBufferSize = size;
  }

signals:
  void textChanged(int column, int line,
		   unsigned int width, unsigned int height);
  void textChanged(); // but we don't know exactly where => redraw everything
  void scrolledLines(int);
  void scrolledColumns(int);

  /**
   * Signalled if a line was cut at the beginning of the
   * internal buffer. Changes offsets. Negative value means
   * 'cut', positive value means 'add'.
   */
  void frontCut(int);

protected:
  void init(unsigned int initialWidth,
	    unsigned int initialHeight,
	    int wordWrapColumn);

  /**
   * ansi y cursor position -> line in buffer
   */
  unsigned int getLine(unsigned int cy) {
    unsigned int line = 0;
    if(buffer.size() < ansiViewHeight)
      line = cursorY;
    else
      line = buffer.size() - ansiViewHeight + cursorY;
    return line;
  }

    void insertOneChar(QChar c, int& minX, int& maxX, int& minY, int& maxY) {
        /* bottleneck!
           the deque operator[] resize and pop_front are the
           most called functions here... how can we possibly avert this?
           lazy parsing?
        */
        assert(cursorX <= ansiViewWidth || (!autoFeedForward && growBuffer));
        assert(cursorY <= ansiViewHeight);

        //qDebug(QString("Character: %1").arg(c));

        unsigned int line = getLine(cursorY);

        if(autoFeedForward &&
           ((cursorX == ansiViewWidth) || // we'll never wrap outside the ansi view
            (wordWrapColumn > 0 &&
             cursorX >= (unsigned int)wordWrapColumn + 1) || // >= because it's possible to jump behind
            (wordWrapColumn < 0 &&             //    the scene via setCursorX()
             cursorX >= (ansiViewWidth + 1 + wordWrapColumn)))) {
            // auto feed (with word wrap)

            if(!growBuffer && cursorY == ansiViewHeight-1)
                return;

            // remember what it looked like...
            // if line >= buffer.size() then there's nothing to wrap, just
            // break the line
            // if cursorX > buffer[line].size(), then the cursor is out of
            // the area, so just break the line
            bool noWordToWrap =
                    (wordWrapColumn == 0   ||
                     line >= buffer.size() || cursorX > buffer[line].size());

            // now create the new line
            cursorX = 0;
            if(cursorY < ansiViewHeight-1) ++cursorY;
            QString qLine;
            for(unsigned int ddd=0; ddd<buffer[line].size(); ++ddd)
                qLine += buffer[line][ddd].getChar();
            //qDebug(QString("End of Line: \"%1\"").arg(qLine));
            ++line;

            if(!noWordToWrap) {
                if(c == QChar(' ')) {
                    createPosition(cursorX, line, minX, maxX, minY, maxY);
                    return; // um, we don't really need to place an empty space
                    // at the beginning of the next line, do we?
                }
//qDebug("Wrapping...");
                for(int c1 = buffer[line-1].size()-1; c1 >= 0; --c1) {
                    if(buffer[line-1][c1].getChar() == QChar(' ')) {
                        //qDebug(QString("Found QChar(' ') at %1").arg(c1));
                        // copy last word
                        for(int c2 = c1+1; c2 < (int)buffer[line-1].size(); ++c2) {
                            createPosition(cursorX, line, minX, maxX, minY, maxY);
                            buffer[line][cursorX] = buffer[line-1][c2];
                            ++cursorX;
                        }
                        unsigned int columns = buffer[line-1].size();
                        buffer[line-1].erase(buffer[line-1].begin() + c1,
                                             buffer[line-1].end());
                        if(!growBuffer)
                            buffer[line-1].resize(columns, defaultAttributes);
                        minX = 0;
                        maxX = ansiViewWidth-1;
                        break;
                    }
                }
            }

            createPosition(cursorX, line, minX, maxX, minY, maxY);
            buffer[line][cursorX] = ColorChar(c, currentAttributes);
            ++cursorX;
            return;
        }

        if(!autoFeedForward && cursorX == ansiViewWidth && !growBuffer) {
            // can't draw anything more
            return;
        }
        if(cursorX == ansiViewWidth) {
            qWarning("*** word wrap did something completely wrong ***");
            //      qDebug(QString("X: %1, WWC: %2, AnsiWidth: %3")
            //	     .arg(cursorX).arg(wordWrapColumn).arg(ansiViewWidth));
            return;
        }

        // default:
        createPosition(cursorX, line, minX, maxX, minY, maxY);
        buffer[line][cursorX] = ColorChar(c, currentAttributes);
        ++cursorX;
    }

  /**
   * if y is popped front, then the argument is changed..
   */
  void createPosition(unsigned int x, unsigned int& y) {
    if(y >= buffer.size()) {
      unsigned int scrolled = y+1-buffer.size();
      unsigned int oldBufferSize = buffer.size();
      buffer.resize(y + 1);

      emit scrolledLines(scrolled);

      if(maxBufferSize > 0 && buffer.size() > maxBufferSize) {
	int diff = 0;
	while(buffer.size() > maxBufferSize &&
	      buffer.size() > oldBufferSize) {
	  buffer.pop_front();
	  --y;
	  --diff;
	}
	emit frontCut(diff);
      }
    }
    if(x >= buffer[y].size())
      buffer[y].resize(x+1);
  }

  void createPosition(unsigned int x, unsigned int& y, int& minX, int& maxX, int& minY, int& maxY) {
    if(minX == -1 || (int)x < minX) minX = (int)x;
    if((int)x > maxX) maxX = (int)x;
    if(minY == -1 || (int)y < minY) minY = (int)y;
    if((int)y > maxY) maxY = (int)y;
    //qDebug(QString("creating Position(%1, %2)").arg(x).arg(y));
    createPosition(x, y);
    //qDebug(QString("         Position(%1, %2)").arg(x).arg(y));
  }

  ColorChar defaultAttributes;
  ColorChar currentAttributes;


  // cursor position:
  // allowed cursor position is specified by ansiViewWidth and ansiViewHeight
  // EXCEPT if wordwrap is disabled and growBuffer == true
  // cursorX may be = ansiViewWidth, this means that the next char
  // should go to the beginning of the next line
  unsigned int cursorX;
  unsigned int cursorY;

  /* ansi view pane in the buffer
   ++++++++++++++++++
   +                +
   +                +
   +                +
   +                +
   +                +
   +                +
   +----------------+\
   +           |    + |
   +           |    +  \ ansiViewHeight: this is needed to be able
   +           |    +  /   to calculate the cursor position when set
   +           |    + |    via an ansi ctrl command
   ++++++++++++++++++/
   \__________/
   ansiViewWidth is needed for word wrapping and stuff
  
   for !growBuffer: ansiViewHeight is always less or equal buffer.size()
                    ansiViewWidth is always less or equal buffer[i].size() 
  */
  unsigned int ansiViewWidth;
  unsigned int ansiViewHeight;

  std::deque< std::vector<ColorChar> > buffer; // [y][x]

  CtrlParser ansiParser;

  // the buffer may be resized when one prints outside
  // of the buffer
  bool growBuffer;

  // if the cursor is on the right side and text is
  // written, do a feed forward automagically
  // 
  bool autoFeedForward;

  // used, if autoFeedForward = true
  // 0: no wordwrap (just continue)
  // n: wrap at n
  // -n: wrap at ansiViewWidth+1-n
  int wordWrapColumn;

  unsigned int maxBufferSize;
};

inline ColorChar TextBuffer::getBufferChar(int column, int line) {
  if(line >= 0 && line < (int)buffer.size()) {
    if(column >= 0 && column < (int)buffer[line].size())
      return buffer[line][column];
  }
  return defaultAttributes;
}

inline unsigned int TextBuffer::getSizeY() { return buffer.size(); }

inline bool TextBuffer::isGrowBuffer() { return growBuffer; }

#endif
