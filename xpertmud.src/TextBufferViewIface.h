// -*- c++ -*-
#ifndef TEXTBUFFERVIEWIFACE_H
#define TEXTBUFFERVIEWIFACE_H

#ifndef ABSTRACT
#  define ABSTRACT 0
#endif

#include <qsize.h>
#include <qfont.h>

class TextBuffer;

class TextBufferViewIface {
public:
  virtual TextBuffer* buffer() = ABSTRACT;
  virtual void switchMouseMode(bool enabled) = ABSTRACT;
  virtual void setScriptingFont(const QFont& newFont) = ABSTRACT;
  virtual QSize sizeForChars(unsigned int x, unsigned int y) = ABSTRACT;
  virtual void scrollLines(int delta) = ABSTRACT;
};

#endif
