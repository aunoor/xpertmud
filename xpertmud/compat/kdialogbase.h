#ifndef KDIALOGBASE_H
#define KDIALOGBASE_H

#include <qdialog.h>
#include <qstring.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qwidgetstack.h>
#include <qlistbox.h>
#include "kiconloader.h"

#include <map>
using std::map;

#include <iostream>
using std::cout;
using std::endl;

class KDialog: public QDialog {
  Q_OBJECT
 public:
  static int spacingHint() {
    return 4;
  }
};

class VPage {
 public:
  VPage() { box = new QVBox(NULL); }
  VPage(QVBox *box, const QString& name,
	const QString& description):
    box(box), name(name), description(description) { 
  }

  QVBox *getBox() { return box; }
  QString getDescription() { return description; }
  QString getName() { return name; }
    
 private:
  QVBox *box;
  QString name;
  QString description;
};

class KDialogBase: public QDialog {
  Q_OBJECT
 public:
  int IconList;
  enum EFace {
    Plain
  };

  enum EButton {
    Ok = 1,
    Cancel = 1<<1,
    Apply = 1<<2,
    Help = 1<<3,
    Default = 1<<4
  };

  KDialogBase(int dummy, const char* name,
	      int buttons, int buttons2, QWidget* parent=NULL,
	      const char* name2=""):
    QDialog(parent, name, true), mainWidget(0), buttonMap(buttons)
    {
      mainWidget = makeMainWidget();
    }

  KDialogBase(QWidget* parent, const char* name,
	      bool sw, const QString& heading,
	      int buttons):
    QDialog(parent, name, true), mainWidget(0), buttonMap(buttons)
    {
      mainWidget = makeMainWidget();
    }

  void setButtonText(EButton b, const QString& text) {}

  QVBox* addVBoxPage(const QString& name, const QString& description,
		     const BarIcon& icon) {
    vpages[name] = VPage(new QVBox(stack), name, description);
    stack->addWidget(vpages[name].getBox(), 0);
    stack->raiseWidget(vpages[name].getBox());
    list->insertItem(name);
    return vpages[name].getBox();
  }

 private:
  QFrame *makeMainWidget() {
    cout << "makeMainWidget" << endl;
    QVBoxLayout *l = new QVBoxLayout(this);
    l->setAutoAdd(TRUE);
    l->setSpacing(5);
    l->setMargin(5);

    QFrame *mainWidget = new QFrame(this);
    upperPart = new QHBox(this);
    sideBar = new QVBox(upperPart);
    list = new QListBox(sideBar);
    connect(list, SIGNAL(selectionChanged(QListBoxItem *)),
	    this, SLOT(slotSelectionChanged(QListBoxItem *)));

    page = new QVBox(upperPart);
    stack = new QWidgetStack(page);

    buttons = new QHBox(this);
    buttons->setSpacing(5);

    if(buttonMap & Ok) {
      QPushButton *ok = new QPushButton("&OK", buttons);
      connect(ok, SIGNAL(clicked()), this, SLOT(slotOk()));
    }
    if(buttonMap & Apply) {
      QPushButton *apply = new QPushButton("&Apply", buttons);
      connect(apply, SIGNAL(clicked()), this, SLOT(slotApply()));
    }
    if(buttonMap & Cancel) {
      QPushButton *cancel = new QPushButton("&Cancel", buttons);
      connect(cancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
    }
    return mainWidget;
  }
 public:
  virtual int exec() {
    list->setSelected(list->topItem(), true);
    return QDialog::exec();
  }

  int spacingHint() { return 5; }

  void enableButtonOK(bool state) {}
  void enableButtonApply(bool state) {}
  void setButtonApplyText(const QString& text) {}

  QWidget* plainPage() { return new QWidget(); }

 protected slots:
  virtual void slotApply() {
  }
  virtual void slotOk() {
    accept();
  }
  virtual void slotCancel() {
    reject();
  }

 private slots:
  void slotSelectionChanged(QListBoxItem *item) {
   cout << "selection changed" << endl;
    QString name = item->text();
    stack->raiseWidget(vpages[name].getBox());
  } 

 protected:
  QFrame *mainWidget;
  QHBox *buttons;
  QHBox *upperPart;
  QVBox *sideBar;
  QListBox *list;
  QVBox *page;
  QWidgetStack *stack;
  int buttonMap;

  map<QString, VPage> vpages;
};

class KStdGuiItem {
 public:
  static QObject* yes() { return new QObject(); }
  static QObject* no() { return new QObject(); }
};

#endif
