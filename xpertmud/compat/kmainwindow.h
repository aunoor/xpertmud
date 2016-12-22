#ifndef KMAINWINDOW_H
#define KMAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include "kstatusbar.h"
#include "ktoolbar.h"
#include "kaction.h"

#ifdef WIN32
# include "Registry.h"
# pragma warning(disable: 4309)
# pragma warning(disable: 4305)
#endif

#include <iostream>
using std::cout;
using std::endl;

class KMainWindow: public QMainWindow {
  Q_OBJECT
 public:
  KMainWindow(QWidget *parent, const QString name, Qt::WindowFlags flags, QList<Qt::WidgetAttribute> a);
  virtual ~KMainWindow();

  virtual KStatusBar* statusBar() { return _bar; }
  virtual QObject* actionCollection() { return &_actionCollection; }

  KToolBar * toolBar(const QString& name) {
    QObject *c = this->findChild<QObject *>(name);
    if(c != NULL) 
      if(c->inherits("KToolBar"))
	return (KToolBar*) c;
    return _toolBar;
  }

  void saveMainWindowSettings(KConfig *config, const QString& name);
  void applyMainWindowSettings(KConfig *config, const QString& name);
  void createGUI(QWidget *);
  static bool canBeRestored(int) { return false; }
  void restore(int) { }
  virtual void closeEvent(QCloseEvent *e);

  virtual bool queryExit() { return true; }
  virtual bool queryClose() { return true; }

  virtual QMenu *createPopupMenu() {return NULL; } //return no PopupMenu for Toolbars

 protected:
  void saveToolBar(KConfig *config, QToolBar *tb);
  void restoreToolBar(KConfig *config, QToolBar *tb);

  KStatusBar *_bar;
  KToolBar *_toolBar;
//  QPtrList<KMainWindow>* memberList;
  QObject _actionCollection;
};

#endif
