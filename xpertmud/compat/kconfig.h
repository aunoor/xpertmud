#ifndef KCONFIG_H
#define KCONFIG_H

#include <QString>
#include <QColor>
#include <QFont>
#include <QSettings>


class KConfig{

public:

    KConfig() {

      QSettings::Format format;
#ifdef Q_OS_WIN
      format = QSettings::IniFormat;
#else
      format = QSettings::NativeFormat;
#endif
      settings = new QSettings(format, QSettings::UserScope, "Xpertmud", "xpertmud");
    }

    virtual ~KConfig() {
      delete settings;
    }

    void setGroup(const QString &group) {
      if (!settings->group().isEmpty()) settings->endGroup();
      if (!group.isEmpty()) settings->beginGroup(group);
    }

    void writeEntry(const QString &name, int val) {
      settings->setValue(name,QVariant(val));
    }

    void writeEntry(const QString &name, bool val) {
      settings->setValue(name,QVariant(val));
    }

    void writeEntry(const QString &name, const QString &val) {
      settings->setValue(name,QVariant(val));
    }

    void writeEntry(const QString &name, const QColor &val) {
      settings->setValue(name, QVariant(val));
    }

    void writeEntry(const QString &name, const QFont &val) {
      settings->setValue(name, QVariant(val));
    }

    QColor readColorEntry(const QString &name, QColor *def) {
      if (!settings->contains(name)) return def==NULL?QColor():*def;

      QVariant res;
      res = settings->value(name);
      if (!res.convert(QVariant::Color)) return def==NULL?QColor():*def;
      return res.value<QColor>();
    }

    QFont readFontEntry(const QString &name, QFont *def) {
      if (!settings->contains(name)) return def==NULL?QFont():*def;

      QVariant res;
      res = settings->value(name);
      if (!res.convert(QVariant::Font)) return def==NULL?QFont():*def;
      return res.value<QFont>();

    }

    QString readEntry(const QString &name, const QString &def) {
      return settings->value(name, def).toString();
    }

    int readNumEntry(const QString &name, int def) {
      return settings->value(name, def).toInt();
    }

    bool readBoolEntry(const QString &name, bool def) {
      return settings->value(name, def).toBool();
    }    
    
    
protected:
    QSettings *settings;

};

#endif
