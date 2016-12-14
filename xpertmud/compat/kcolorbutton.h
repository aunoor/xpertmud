#ifndef KCOLORBUTTON_H
#define KCOLORBUTTON_H

#include <qpushbutton.h>
#include <qcolordialog.h>

class KColorButton: public QPushButton {
  Q_OBJECT
 public:
  KColorButton(QWidget* parent):
    QPushButton(parent) {
    connect(this, SIGNAL(clicked()), this, SLOT(slotClicked()));
  }
  void setColor(QColor col) {
    this->col = col;
    QPixmap fillMap(30, 10);
    fillMap.fill(col);
    setPixmap(fillMap);
  }
  QColor color() { return col; }

 private slots:
  void slotClicked() {
   QColor c = QColorDialog::getColor(col);
   setColor(c);
  }
 private:
  QColor col;
};

#endif
