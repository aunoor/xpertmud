#ifndef KCONFIG_H
#define KCONFIG_H

#include <qstring.h>
#include <qcolor.h>
#include <qfont.h>
#ifdef WIN32
#include <qsettings.h>
#endif

class KConfig {
#ifdef WIN32
 protected:
  QSettings settings;
#endif
 public:
  void setGroup(const QString& group) {
    this->group = group;
  }
  void writeEntry(const QString& name, int val) {
#ifdef WIN32
    settings.writeEntry(getKeyName(name),val);
#endif
  }
  void writeEntry(const QString& name, const QString& val) {
#ifdef WIN32
    settings.writeEntry(getKeyName(name),val);
#endif
  }
  void writeEntry(const QString& name, const QColor& val) {
#ifdef WIN32
    settings.writeEntry(getKeyName(name),val.name());
#endif
  }
  void writeEntry(const QString& name, const QFont& val) {
#ifdef WIN32
    settings.writeEntry(getKeyName(name),fontToString(val));
#endif
  }
  QColor readColorEntry(const QString& name, QColor* def) {
#ifdef WIN32
    QString sdef = def->name();
    QString r = settings.readEntry(getKeyName(name), sdef);
    return QColor(r);
#else
    return *def;
#endif
  }
  QFont readFontEntry(const QString& name, QFont* def) {
#ifdef WIN32
    QString defRep = fontToString(*def);
    QString rep = settings.readEntry(getKeyName(name), defRep);
    return stringToFont(rep);
#else
    return *def;
#endif
  }
  QString readEntry(const QString& name, const QString& def="") {
#ifdef WIN32
    return settings.readEntry(getKeyName(name), def);
#else
    return def;
#endif
  }
  int readNumEntry(const QString& name, int def) {
#ifdef WIN32
    return settings.readNumEntry(getKeyName(name), def);
#else
    return def;
#endif
  }
  bool readBoolEntry(const QString& name, bool def=true) {
#ifdef WIN32
    return settings.readNumEntry(getKeyName(name), def);
#else
    return def;
#endif
  }
 private:
#ifdef WIN32
  QString getKeyName(const QString& entry) {
    QString keyName = "/Xpertmud/";
    if(group != "") {
      keyName += group;
      keyName += '/';
    }
    keyName += entry;
    return keyName;
  }
  QString fontToString(const QFont& font) {
    //    cout << font.family() << " " << font.pointSize()
    //	<< " " << font.weight() << " " << (int)font.italic();
    QString s;
    QTextStream str(&s, IO_WriteOnly);
    str << font.family() << " || " << font.pointSize()
	<< " " << font.weight() << " " << (int)font.italic();
    //    cout << "Made: " << s.latin1() << endl;
    return s;
  }
  QFont stringToFont(QString s) {
    QTextStream str(&s, IO_ReadOnly);
    QString family;
    int ps, weight;
    int italic;
    QString tmp;
    int firstSpace = 2;
    do {
      if(firstSpace == 0) family += " ";
      family += tmp;
      str >> tmp;
      --firstSpace;
    } while(tmp != "||");
    str >> ps >> weight >> italic;
    //    cout << "stringToFont: Family = \"" << family.latin1() << "\"" << endl;
    return QFont(family, ps, weight, italic);
  }
#endif
  QString group;
};

#endif
