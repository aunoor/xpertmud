#ifndef KSTATUSBAR_H
#define KSTATUSBAR_H

#include <qstatusbar.h>
#include <qlabel.h>

class KStatusBar {
 public:
  KStatusBar(QStatusBar* bar):
    bar(bar) {}

  void insertItem(const QString& text,int id,int stretch,
		  bool permanent=false) {
    QLabel *nw = new QLabel(text, bar, QString::number(id));
    bar->addWidget(nw, stretch, permanent);
  }
  
  void changeItem(const QString& item, int id) {
    QLabel *w = (QLabel*)bar->child(QString::number(id));
    if(w != NULL) {
      w->setText(item);
    }
  }
  
  void clear() { bar->clear(); }

  void show() { bar->show(); }

  void hide() { bar->hide(); }
  bool isHidden() { return bar->isHidden(); }

 private:
  QStatusBar *bar;
};

#endif
