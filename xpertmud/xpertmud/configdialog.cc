#include "configdialog.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qcombobox.h>

#include <klocale.h>
#include <kdialogbase.h>
#include <kiconloader.h>
#include <kcolorbutton.h>
#include <kfontdialog.h>


ConfigDialog::ConfigDialog(QWidget * parent,const char *name):
  KDialogBase(KDialogBase::IconList,i18n("Configure Xpertmud"),
	      KDialogBase::Ok | KDialogBase::Help | 
	      KDialogBase::Cancel | KDialogBase::Default,
	      KDialogBase::Ok, parent,name) {

  addColorPage();
  addHistoryPage();
  addScriptingPage();
  addFontPage();
  addInputPage();
}

ConfigDialog::~ConfigDialog() {}


void ConfigDialog::addColorPage() {
  QVBox * page = addVBoxPage(i18n("Colors"),QString::null,
			     BarIcon("colorize", KIcon::SizeMedium) );

  QWidget * colorBox=new QWidget(page);
  QGridLayout* glay = new QGridLayout( colorBox, 9, 4, 0, KDialog::spacingHint());

  {   
    QLabel * label = new QLabel(i18n("Default BG"),colorBox);
    label->setAlignment(AlignVCenter | AlignRight);
    glay->addWidget(label,0,0);
    colorButton[16]=new KColorButton(colorBox);
    glay->addWidget(colorButton[16],0,1);
  }
  {
    colorButton[17]=new KColorButton(colorBox);
    glay->addWidget(colorButton[17],0,2);
    QLabel * label = new QLabel(i18n("Default FG"),colorBox);
    label->setAlignment(AlignVCenter | AlignLeft);
    glay->addWidget(label,0,3);
  }
  for (int i=0; i<8; ++i) {
    QLabel * label = new QLabel(QString::number(i),colorBox);
    label->setAlignment(AlignVCenter | AlignRight);
    glay->addWidget(label,i+1,0);
    colorButton[i]=new KColorButton(colorBox);
    glay->addWidget(colorButton[i],i+1,1);
  }  
  
  {for (int i=8; i<16; ++i) {
    colorButton[i]=new KColorButton(colorBox);
    glay->addWidget(colorButton[i],i-7,2);
    QLabel * label = new QLabel(QString::number(i),colorBox);
    label->setAlignment(AlignVCenter | AlignLeft);
    glay->addWidget(label,i-7,3);
  }}  
 
}


void ConfigDialog::addHistoryPage() {
  QVBox * page = addVBoxPage(i18n("History"),QString::null,
		     BarIcon("history", KIcon::SizeMedium) );
  QWidget * histBox=new QWidget(page);
  QGridLayout* glay = new QGridLayout( histBox, 4, 2, 0, KDialog::spacingHint());
  QLabel * label;
  label=new QLabel(i18n("Default History Size:"),histBox);
  label->setAlignment(AlignVCenter);
  glay->addMultiCellWidget(label,0,0,0,1);


  historyCheckBox=new QCheckBox(i18n("limit buffer size"),histBox);
  historyCheckBox->setChecked(true);
  glay->addMultiCellWidget(historyCheckBox,1,1,0,1);

  historySpinBox=new QSpinBox(1,1<<30,100,histBox);
  historySpinBox->setValue(2000);
  glay->addWidget(historySpinBox,2,0);

  label=new QLabel(i18n("lines"),histBox);
  glay->addWidget(label,2,1);
  
  
  //  glay->addWidget(new QWidget(histBox),3,0);
  glay->setRowStretch(0,0);
  glay->setRowStretch(1,0);
  glay->setRowStretch(2,0);
  glay->setRowStretch(3,1);
  glay->setColStretch(0,0);
  glay->setColStretch(1,1);

  connect(historyCheckBox,SIGNAL(toggled(bool)),
	  historySpinBox, SLOT(setEnabled(bool)));


  
}
void ConfigDialog::addScriptingPage() {
  QVBox * page = addVBoxPage(i18n("Scripting"),QString::null,
		     BarIcon("pencil", KIcon::SizeMedium) );
  QWidget * scriptBox=new QWidget(page);
  QVBoxLayout* lay = new QVBoxLayout( scriptBox, 0, KDialog::spacingHint());
  QLabel * label;
  label=new QLabel(i18n("Default Scripting engine:"),scriptBox);
  label->setAlignment(AlignVCenter);
  lay->addWidget(label);

  defaultScriptingBox=new QComboBox(true,scriptBox);
  defaultScriptingBox->setAutoCompletion(true);
  defaultScriptingBox->setDuplicatesEnabled(false);
  defaultScriptingBox->setEditable(true);
  lay->addWidget(defaultScriptingBox);
  
  lay->addStretch(1);
}

void ConfigDialog::addFontPage() {
  QVBox * page = addVBoxPage(i18n("Font"),QString::null,
			     BarIcon("fonts", KIcon::SizeMedium) );
  fontChooser=new KFontChooser(page,"fontChooser",true);

}
void ConfigDialog::addInputPage() {
  QVBox * page = addVBoxPage(i18n("Input"),QString::null,
			     BarIcon("editclear", KIcon::SizeMedium) );

  // TODO: layout
  commandRetentionCheckBox=new QCheckBox(i18n("enable command retention"),page);
}

void ConfigDialog::setColors(const QColor * colors) {
  for (int i=0; i<18; ++i) {
    colorButton[i]->setColor(colors[i]);
  }
}

void ConfigDialog::storeColors(QColor * colors) const {
  for (int i=0; i<18; ++i) {
    colors[i]=colorButton[i]->color();
  }
}

void ConfigDialog::setDefaultFont(const QFont & f) {
  fontChooser->setFont(f,true);
}

QFont ConfigDialog::getDefaultFont() const {
  return fontChooser->font();
}

void ConfigDialog::slotDefault() {
#ifdef WIN32
    QFont myFont("fixedsys");
    myFont.setFixedPitch(true);
    myFont.setPointSize(9);
#else
    QFont myFont("Courier");
    myFont.setFixedPitch(true);
    myFont.setStyleHint(QFont::Courier);
#endif
  setDefaultFont(myFont);
  setColors(base_color_table);
  setHistSize(0);
  setDefaultLanguage("perl");
}

void ConfigDialog::setHistSize(int lines) {
  if (lines >0) {
    historyCheckBox->setChecked(true);
    historySpinBox->setValue(lines);
  } else {
    historyCheckBox->setChecked(false);
  }
}

int ConfigDialog::getHistSize() const {
  if (historyCheckBox->isChecked()) {
    return historySpinBox->value();
  } else {
    return 0;
  }
}

void ConfigDialog::setScriptingLangs(const QStringList & sl) {
  defaultScriptingBox->insertStringList(sl);
}

void ConfigDialog::setDefaultLanguage(const QString & lang) {
  defaultScriptingBox->setEditText(lang);
}

QString ConfigDialog::getDefaultLanguage() const {
  return defaultScriptingBox->currentText();
}

void ConfigDialog::setCommandRetention(bool cr) {
  commandRetentionCheckBox->setChecked(cr);
}

bool ConfigDialog::getCommandRetention() const {
  return commandRetentionCheckBox->isChecked();
}
