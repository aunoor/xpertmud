#ifndef KDIALOGBASE_H
#define KDIALOGBASE_H

#include <QDialog>
#include <QString>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
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

class QVBox : public QWidget {
public:
    QVBox(QWidget *parent):QWidget(parent) {
      QVBoxLayout *layout = new QVBoxLayout();
      this->setLayout(layout);
    }
    virtual void childEvent(QChildEvent *event) {
      if (event->type()==QEvent::ChildAdded) {
        if (event->child()->isWidgetType()) {
          layout()->addWidget((QWidget*)event->child());
        }
      }
      QWidget::childEvent(event);
    }
};

class QHBox : public QWidget {
public:
    QHBox(QWidget *parent):QWidget(parent) {
      QHBoxLayout *layout = new QHBoxLayout();
      this->setLayout(layout);
    }
    void setSpacing(int value) {
      layout()->setSpacing(value);
    }
    virtual void childEvent(QChildEvent *event) {
      if (event->type()==QEvent::ChildAdded) {
        if (event->child()->isWidgetType()) {
          layout()->addWidget((QWidget*)event->child());
        }
      }
      QWidget::childEvent(event);
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

  KDialogBase(const QString title,
	      int buttons, int buttons2, QWidget* parent=NULL,
	      const QString name=QString()):
    QDialog(parent), mainWidget(0), buttonMap(buttons)
    {
      setModal(true);
      setWindowTitle(title);
      setObjectName(QString(name));
      mainWidget = makeMainWidget();
    }

  KDialogBase(QWidget* parent, const QString title,
	      bool sw, const QString& heading,
	      int buttons):
    QDialog(parent), mainWidget(0), buttonMap(buttons)
    {
      setModal(true);
      setWindowTitle(title);
      mainWidget = makeMainWidget();
    }

  void setButtonText(EButton b, const QString& text) {}

  QVBox* addVBoxPage(const QString& name, const QString& description,
		     const BarIcon& icon) {
    vpages[name] = VPage(new QVBox(stack), name, description);
    stack->insertWidget(0, vpages[name].getBox());
    stack->setCurrentWidget(vpages[name].getBox());
    list->addItem(name);
    return vpages[name].getBox();
  }

 private:
    QFrame *makeMainWidget() {
//    cout << "makeMainWidget" << endl;
    QVBoxLayout *l = new QVBoxLayout(this);
    //l->setAutoAdd(TRUE);
    l->setSpacing(5);
    l->setMargin(5);

    QFrame *mainWidget = new QFrame(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    l->addWidget(mainWidget);

    upperPart = new QHBox(this);
    mainLayout->addWidget(upperPart);

    sideBar = new QVBox(upperPart);
    list = new QListWidget(sideBar);
    connect(list, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
	    this, SLOT(slotSelectionChanged(QListWidgetItem *)));

    page = new QVBox(upperPart);
    stack = new QStackedWidget(page);

    buttons = new QHBox(this);
    buttons->setSpacing(5);
    mainLayout->addWidget(buttons);

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
    list->setCurrentRow(0);
    //list->setSelected(list->topItem(), true);
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
  void slotSelectionChanged(QListWidgetItem *item) {
   cout << "selection changed" << endl;
    QString name = item->text();
    stack->setCurrentWidget(vpages[name].getBox());
  } 

 protected:
  QFrame *mainWidget;
  QHBox *buttons;
  QHBox *upperPart;
  QVBox *sideBar;
  QListWidget *list;
  QVBox *page;
  QStackedWidget *stack;
  int buttonMap;

  map<QString, VPage> vpages;
};

class KStdGuiItem {
 public:
  static QObject* yes() { return new QObject(); }
  static QObject* no() { return new QObject(); }
};

#endif
