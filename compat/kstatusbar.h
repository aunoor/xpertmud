#ifndef KSTATUSBAR_H
#define KSTATUSBAR_H

#include <QStatusBar>
#include <QLabel>

class KStatusBar {
 public:
  KStatusBar(QStatusBar* bar):
    bar(bar) {}

  void insertItem(const QString& text,int id,int stretch,
		  bool permanent=false) {
    QLabel *nw = new QLabel(text, bar);
    nw->setObjectName(QString::number(id));
    if (permanent) bar->addPermanentWidget(nw, stretch);
    else bar->addWidget(nw, stretch);
  }
  
  void changeItem(const QString& item, int id) {
    QLabel *w = bar->findChild<QLabel *>(QString::number(id));
    if(w != NULL) {
      w->setText(item);
    }
  }
  
  void clear() { bar->clearMessage(); }

  void show() { bar->show(); }

  void hide() { bar->hide(); }
  bool isHidden() { return bar->isHidden(); }

 private:
  QStatusBar *bar;
};

#endif
