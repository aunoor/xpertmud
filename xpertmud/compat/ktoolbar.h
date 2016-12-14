#ifndef KTOOLBAR_H
#define KTOOLBAR_H

#include <qtoolbar.h>
#include <map>
#include <qmap.h>
#include <qstring.h>
#include <qmainwindow.h>

#include <iostream>
using std::cout;
using std::endl;

class KToolBar: public QToolBar {
  Q_OBJECT
 public:
  typedef QMap<int, QWidget* > Id2WidgetMap;

  KToolBar() {}
  KToolBar(const QString& text, QMainWindow* parent,
	   QMainWindow::ToolBarDock dock= QMainWindow::Top,
	   bool newline=false, const char *name=0):
    QToolBar(text, parent, dock, newline, name) {}

  KToolBar(QMainWindow* parent, const char *name, bool, bool):
    QToolBar(parent, name) {}
  virtual ~KToolBar() {}

  void insertWidget(int id, int, QWidget*w, int) {
    cout << "inserted... " << id << endl;
    cout << "hmmm" << (long)w << endl;
    id2widget.insert(id, w);
    cout << "done" << endl;
  }

  void setItemAutoSized(int id, bool enable) {
    Id2WidgetMap::Iterator it = id2widget.find( id );
    QWidget *w = (( it == id2widget.end() ) ? 0 : (*it));
    if(w && enable) {
      cout << "yep!" << endl;
      setStretchableWidget(w);
    }
  }

  void removeItem(int id) {
  }

 protected:
  Id2WidgetMap id2widget;

};

class KEditToolbar: public QWidget {
  Q_OBJECT
 public:
  KEditToolbar(QObject* collection) {
  }

  bool exec() {
    return false;
  }

 signals:
  void newToolbarConfig(); 
 
};

#endif
