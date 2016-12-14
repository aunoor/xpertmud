#ifndef KFONTDIALOG_H
#define KFONTDIALOG_H

#include <qfontdialog.h>
#include <qpushbutton.h>
#include <qvbox.h>

class KFontDialog {
 public:

  enum AState {
    Rejected = 0,
    Accepted = 1
  };

  static int getFont(QFont &font, bool b, QWidget *parent) {
    bool accepted;
    font = QFontDialog::getFont(&accepted, font, parent);
    if(accepted) {
      return Accepted;
    } else {
      return Rejected;
    }
  }

};

class KFontChooser: public QWidget {
  Q_OBJECT
 public:
  KFontChooser() {}
  KFontChooser(QWidget *parent, const char *name, bool):
    QWidget(parent, name) {
    QPushButton *button = new QPushButton("Change Font", this);
    connect(button, SIGNAL(clicked()), this, SLOT(slotClicked()));
  }

  void setFont(const QFont& font, bool) {
    _font = font;
  }
  QFont font() { return _font; }

 private slots:
  void slotClicked() {
   QFont newFont = _font;
   if(KFontDialog::getFont(newFont, true, NULL) == KFontDialog::Accepted)
     setFont(newFont, true);
  }

 private:
  QFont _font;
};

#endif
