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
       Qt::ToolBarArea dock = Qt::TopToolBarArea,
	   bool newline=false, const char *name=0);

  KToolBar(QMainWindow* parent, const char *name, bool, bool);

  virtual ~KToolBar() {}

  void insertWidget(int id, int, QWidget*w, int);

  void setItemAutoSized(int id, bool enable);

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
