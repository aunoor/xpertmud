#ifndef KURL_H
#define KURL_H

#include <qstring.h>
#ifdef WIN32
#pragma warning( disable : 4100 )
#endif

class KURL {
 public:
  KURL(const QString& str) {
    getFromString(str);
  }

  KURL() {}
  QString url() const { return dir+'/'+name; }
  void setProtocol(const QString& pro) {}
  void setPath(const QString& p) {
    dir = p;
  }
  void setQuery(const QString& pro) {}
  void setFileName(const QString& fn) {
    name = fn;
  }
  static QString encode_string(const QString& string) {
    return string;
  }

  KURL& operator=(const QString& s) {
    getFromString(s);
    return *this;
  }

  bool isEmpty() const { return name.isEmpty(); }
  bool isLocalFile() const { return true; }

  QString fileName() const { 
    return name;
  }
  QString directory() const { 
    return dir;
  }

 private:
  void getFromString(const QString& str) {
    int pos = str.findRev('/', -2); // ignore a trailing '/'
    name = str.mid(pos+1);
    if(pos != -1)
      dir = str.left(pos);
    else
      dir = "";
  }
  QString dir;
  QString name;
};

#endif
