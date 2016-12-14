// -*- c++ -*-
#ifndef CTRLPARSER_H
#define CTRLPARSER_H

#ifdef WIN32
#pragma warning ( disable:4786 )
#endif

#include <qstring.h> // unicode ...
#include <map>
#include <vector>

#ifndef ABSTRACT
#  define ABSTRACT 0
#endif

class CtrlCallbackIface {
public:
  virtual void setBGColor(int) = ABSTRACT;
  virtual void setFGColor(int) = ABSTRACT;
  virtual void resetBGColor() = ABSTRACT;
  virtual void resetFGColor() = ABSTRACT;
  virtual void resetAttributes() = ABSTRACT;
  virtual void setIntense(bool) = ABSTRACT;
  virtual void setFaint(bool) = ABSTRACT;
  virtual void setItalic(bool) = ABSTRACT;
  virtual void setUnderline(bool) = ABSTRACT;
  virtual void setBlinking(bool) = ABSTRACT;
  virtual void setRapidBlinking(bool) = ABSTRACT;
  virtual void setReverse(bool) = ABSTRACT;
  virtual void setConcealed(bool) = ABSTRACT;
  virtual void setStrikeout(bool) = ABSTRACT;
  virtual void setFraktur(bool) = ABSTRACT;
  virtual void setDoubleUnderline(bool) = ABSTRACT;
  virtual void setFramed(bool) = ABSTRACT;
  virtual void setEncircled(bool) = ABSTRACT;
  virtual void setOverline(bool) = ABSTRACT;
  virtual void clearEOL() = ABSTRACT;
  virtual void clearBOL() = ABSTRACT;
  virtual void clearLine() = ABSTRACT;
  virtual void clearEOS() = ABSTRACT;
  virtual void clearBOS() = ABSTRACT;
  virtual void clearWindow() = ABSTRACT;
  virtual void setCursor(unsigned int, unsigned int) = ABSTRACT;
  virtual unsigned int getCursorX() = ABSTRACT;
  virtual unsigned int getCursorY() = ABSTRACT;
  virtual void newline() = ABSTRACT;
  virtual void tab() = ABSTRACT;
  virtual void backspace() = ABSTRACT;
  virtual void beep() = ABSTRACT;
  virtual void carriageReturn() = ABSTRACT;

  virtual void printRaw(const QString& text) = ABSTRACT;
};

class CtrlParser {
  typedef std::map< int, std::vector<int> > defaultParamMapT;
  enum EState {
    SNORMAL, SESC, 
    SCHAR, SSHARP, STHRICE, STHRICE_PARAM,
    SATTR1, SSPACE1,
    SATTR2, SSPACE2,
    SATTRN, SSPACEN
  };
  enum EMode {
    PARSE,
    STRIP
  };
public:
  CtrlParser();
  CtrlParser(CtrlCallbackIface *callback);
  void parse(const QString& str);

  QString returnStripped() {
    return stripped;
  }
  QString returnColored() {
    return colored;
  }

private:
  inline bool in(const QChar c, char *set) {
    while(*set != 0) {
      if(c == *set) { return true; }
      ++set;
    }
    return false;
  }
  void execute(const char command, EState execState);
  void print(const QChar* from,
	     const QChar* to);
  void putparam(const QChar* from,
		const QChar* to);
  void initDefaultParams();

  inline QString encodeCharset(const QChar* from, 
			       const QChar* to) {
    if(vt100special) {
      QString ret;
#if QT_VERSION < 300
      for(const QChar* it=from; it!=to; ++it) {
	ret.append(encodeCharset(it));
      }
#else
      ret.setLength(to-from);
      for(int i=0; i<to-from; ++i) {
	ret[i] = encodeCharset(from+i);
      }
#endif
      return ret;
    }
    return QString(from, to-from);
  }
  inline QChar encodeCharset(const QChar* ch) {
    if(vt100special &&  0x5f <= ch->unicode() && 
       ch->unicode() <= 0x7e) 
      return vt100_graphics[ch->unicode()-0x5f];
    return *ch;
  }

  EState state;
  std::vector< std::pair<int, bool> > param;
  defaultParamMapT defaultParams;

  EMode mode;

  // Mode: Parse
  CtrlCallbackIface *callback;
  
  // Mode: Strip
  QString stripped;
  QString colored;
  char fgColor;
  char bgColor;

  QString unparsed;
  
  bool vt100special;

  static QChar vt100_graphics[32];
};

#endif
