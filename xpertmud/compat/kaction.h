#ifndef KACTION_H
#define KACTION_H

#include "kpopupmenu.h"
#include "kurl.h"
#include "kconfig.h"
#include <QAction>
#include <QObject>
#include <iostream>
using std::cout;
using std::endl;

class KAction: public QAction {
  Q_OBJECT
 public:
  KAction():
  //QAction ( const QString & text, const QString & menuText, QKeySequence accel, QObject * parent, const char * name = 0, bool toggle = FALSE )  (obsolete)
    QAction(NULL) {}
    //QAction("", "", 0, NULL, "", false) {}

  KAction( const QString& text, int accel,
  	   const QObject* receiver, const char* slot,
	   QObject* parent, const char* name="",
	   bool toggle=false):
    QAction(text, parent) {
    //QAction(text, text, accel, parent, name, toggle) {
      this->toggled(toggle);
      this->setShortcut(accel);
      this->setObjectName(QString(name));
      connect(this, SIGNAL(activated()), receiver, slot);
    }

  KAction( const QString& text, int accel, 
	   QObject* parent = 0, const char* name = 0,
	   bool toggle=false):
    QAction(text, parent) {
    //QAction(text, text, accel, parent, name, toggle) {
      std::cout << (long)this << std::endl;
      std::cout << (long)parent << std::endl;
      std::cout << name << std::endl;
      this->toggled(toggle);
      this->setShortcut(accel);
      this->setObjectName(QString(name));
    }

  virtual ~KAction() {}

  static int getToolButtonID() {
    return 0;
  }

  int containerCount() { return 1; }
  int findContainer(QWidget*) { return 0; }
  int menuId(int) { return 0; }
  void removeContainer(int) {}

  virtual int plug( QWidget *w, int index = -1 ) {
    w->addAction(this);
    return true;
  }
  
  virtual void unplug( QWidget *w ) {}
  virtual int itemId(int) { return 0; }

};

class KRecentFilesAction: public KAction {
  Q_OBJECT
 public:
  void saveEntries(KConfig* config, const QString &name) {}
  void loadEntries(KConfig* config, const QString &name) {}
  void addURL(const KURL& url) {}
  void removeURL(const KURL& url) {}
};

class KToggleAction: public KAction {
  Q_OBJECT
 public:
  KToggleAction( const QString& text, int accel,
		 const QObject* receiver, const char* slot,
		 QObject* parent, const char* name ):
    KAction(text, accel, receiver, slot, parent, name, true) {}

  //KToggleAction() {}

  bool isChecked() { return isChecked(); }
  void setChecked(bool enable) { setChecked(enable); }
};

class KActionMenu: public KAction {
  Q_OBJECT
 public:
  KActionMenu( const QString& text, QObject* parent = 0,
	       const char* name = 0 ): 
    KAction(text, 0, parent, name),
    menu(new KPopupMenu(NULL)) {
    connect(menu, SIGNAL(aboutToShow()),
	    this, SIGNAL(aboutToShow()));
  }
  ~KActionMenu() {
   delete menu;
  }

  KPopupMenu* popupMenu() {
    return menu;
  }

  void insert(KAction *action) {
    action->plug(menu, 0);
  }

 signals:
  void aboutToShow();

 private:
  KPopupMenu * menu;
};

#endif
