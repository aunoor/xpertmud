// -*- c++ -*-
#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <qobject.h>
#include <qlabel.h>
#include <kdialogbase.h>
#include <kcolorbutton.h>

#include "ColorChar.h"
class QSpinBox;
class QCheckBox;
class QComboBox;
class QStringList;
class KFontChooser;

class ConfigDialog:public KDialogBase {
  Q_OBJECT
public:
  ConfigDialog( QWidget * parent=0,const char *name=0);
  ~ConfigDialog(); 


  void setColors(const QColor * colors);
  void storeColors(QColor * colors) const;

  int getHistSize() const;
  void setHistSize(int lines);

  void setScriptingLangs(const QStringList &);
  void setDefaultLanguage(const QString &);
  QString getDefaultLanguage() const;

  void setDefaultFont(const QFont &);
  QFont getDefaultFont() const;

  void setCommandRetention(bool);
  bool getCommandRetention() const;

protected slots:
  void slotDefault();

protected:
  void addColorPage();
  void addHistoryPage();
  void addScriptingPage();
  void addFontPage();
  void addInputPage();
private:
  KColorButton * colorButton[18];
  QSpinBox * historySpinBox;
  QCheckBox * historyCheckBox;
  QComboBox * defaultScriptingBox;
  KFontChooser * fontChooser;
  QCheckBox * commandRetentionCheckBox;
};

#endif
