// -*- c++ -*-
#ifndef COLORCHAR_H
#define COLORCHAR_H

#include <qstring.h>
#include <qcolor.h>

class ColorChar {
public:
  enum EAttribute {
    A_NONE =        0,
    A_INTENSE =    (1<<0),
    A_FAINT =      (1<<1),
    A_ITALIC =     (1<<2),
    A_UNDERLINE =  (1<<3),
    A_BLINK =      (1<<4),
    A_RAPIDBLINK = (1<<5),
    A_REVERSE =    (1<<6),
    A_CONCEALED =  (1<<7),
    A_STRIKEOUT =  (1<<8),

    A_FRAKTUR =    (1<<9),
    A_DUNDERLINE = (1<<10),
    A_FRAMED =     (1<<11),
    A_ENCIRCLED =  (1<<12),
    A_OVERLINE =   (1<<13),

    A_DEFAULTFG =  (1<<14),
    A_DEFAULTBG =  (1<<15),

    A_BOLD = (1<<16)
  };

  ColorChar(QChar ch = QChar(' '), 
	    unsigned char fg = 7,  
	    unsigned char bg = 0,  
	    unsigned short attr = A_DEFAULTFG | A_DEFAULTBG):
    ch(ch), fg(fg), bg(bg), attr(attr) {}

  ColorChar(QChar ch, ColorChar col):
    ch(ch),fg(col.fg),bg(col.bg),attr(col.attr) {}

  inline QChar getChar() const {
    return ch;
  }

  inline void setChar(QChar c) {
    ch = c;
  }

  inline int getFg() const { 
    if(checkAttribute(A_INTENSE) && fg < 8) {
      return fg | 0x08; 
    } else {
      return fg;
    }
  }

  inline int getBg() const { return bg; }

  inline void setFg(int color) { 
    setAttribute(A_DEFAULTFG, false);
    fg = color;
  }

  inline void setBg(int color) {
    setAttribute(A_DEFAULTBG, false);
    bg = color;
  }

  inline bool checkAttribute(EAttribute a) const {
    return attr & a;
  }

  inline void setAttribute(EAttribute a, bool to=true) {
    if(to)
      attr |= a;
    else
      attr &= ~a;
  }

  inline void resetAttribute(EAttribute a) {
    attr = A_DEFAULTFG | A_DEFAULTBG;
  }

  inline bool operator<(const ColorChar& b) const {
    if(ch < b.ch) return true;
    else if(ch == b.ch) {
      if(fg < b.fg) return true;
      else if(fg == b.fg) {
	if(bg < b.bg) return true;
	else if(bg == b.bg) {
	  if(attr < b.attr) return true;
	}
      }
    }
    return false;
  }

  inline bool operator==(const ColorChar& b) const {
    return ((ch == b.ch) &&
	    (fg == b.fg) &&
	    (bg == b.bg) &&
	    (attr == b.attr));
  }

  inline bool operator!=(const ColorChar& b) const {
    return ((ch != b.ch) ||
	    (fg != b.fg) ||
	    (bg != b.bg) ||
	    (attr != b.attr));
  }

  inline bool equalColorsAttr(const ColorChar& b) const {
    return ((fg == b.fg) &&
	    (bg == b.bg) &&
	    (attr == b.attr));
  }

  inline QString getHashKey() const {
    // TODO: faster method?
    return QString("%1-%2-%3-%4").arg(ch).arg((int)fg).arg((int)bg).arg(attr);
  }

protected:
  // <begin 2 int> one int's not enough, let's
  //               head for 2 ints :) 1.5 ints is
  //               possible at the moment, but then
  //               the stuff is not int-aligned...
  //               And 2 ints are ready for the future 
  //               (there are many more attributes in the
  //                ECMA spec...)
  QChar ch;
  // 0 - 7 normal, 8 - 15 intense, 16 - 231 6x6x6 color cube
  //   with red*36 + green*6 + blue
  // 232-255 greyscale, 232: very dark grey, 255 very light grey
  unsigned char fg; 
  unsigned char bg;
  unsigned int attr;
  // <end 2 int>

};

extern const unsigned int base_color_table_size;
extern const QColor base_color_table[];


#endif
