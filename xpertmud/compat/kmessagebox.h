#ifndef KMESSAGEBOX_H
#define KMESSAGEBOX_H

#ifdef WIN32
#pragma warning( disable : 4100 )
#endif

#include <qmessagebox.h>

class KMessageBox: public QMessageBox {
  Q_OBJECT
 public:
  static void error(QWidget* parent, const QString& text,
		    const QString &caption = "") {
    QMessageBox b(caption, text, QMessageBox::Critical,
		  QMessageBox::Ok, QMessageBox::NoButton,
		  QMessageBox::NoButton, parent, NULL);
    b.exec();
  }

  static int warningYesNo(QWidget* parent,
			   const QString& text,
			   const QString& s,
			   QObject *yes,
			   QObject *no,
			   const char *name) {
    QMessageBox b(s, text, QMessageBox::Warning,
		  QMessageBox::Yes, QMessageBox::No,
		  QMessageBox::NoButton, parent, name);
    return b.exec();
  }
			   
  static int warningYesNoCancel(QWidget* parent,
			   const QString& text,
			   const QString& s) {
    QMessageBox b(s, text, QMessageBox::Warning,
		  QMessageBox::Yes, QMessageBox::No,
		  QMessageBox::Cancel, parent, NULL);
    return b.exec();
  }
};

#endif
