// -*- c++ -*-
#ifndef CHARSETPARSER_H
#define CHARSETPARSER_H

#include <qstring.h>
#include <cassert>

class CharSetParser {
  enum EEncoding {
    EE_None,
    EE_ASCII,
    EE_UTF8,
    EE_LATIN1
  };
public:
  CharSetParser(): idle(true), enc(EE_None) {
  }

  // returns weather a full encoding
  // change was parsed...
  // this stuff is extremely optimized,
  // use with care (look at Connection.cc
  // to see how to use it, it's the only
  // way to use it ;-)
  bool add(const QChar& c) {
    if(idle) {
      buffer.truncate(0);
    }
    if(buffer.isEmpty()) {
      assert(idle);
      if(c == (char)27) {
	idle = false;
      }
    } else if(buffer.length() == 1) {
      if(c == '%' || c == '-') {
	; // go ahead
      } else {
	idle = true;
      }
    } else if(buffer.length() == 2) {
      if(buffer[1] == '%') {
	if(c == 'G') {
	  enc = EE_UTF8;
	  idle = true;
	  return true;
	} else if(c == '@') {
	  enc = EE_ASCII;
	  idle = true;
	  return true;
	}
      } else if(buffer[1] == '-') {
	if(c == 'A' && enc == EE_ASCII) {
	  enc = EE_LATIN1;
	  idle = true;
	  return true;
	}
      }
      idle = true;
    }
    buffer += c;
    return false;
  }

  QString readUntilIdle() {
    return buffer;
  }

  bool isIdle() {
    return idle;
  }

  QString encoding() {
    switch(enc) {
    case EE_UTF8:
      return "UTF-8";
    case EE_ASCII:
    case EE_LATIN1:
    default:
      return "ISO-8859-1";
    }
  }

private:
  bool idle;
  EEncoding enc;
  QString buffer;
};

#endif
