#ifndef KMESSAGEBOX_H
#define KMESSAGEBOX_H

#ifdef WIN32
#pragma warning( disable : 4100 )
#endif

#include <QMessageBox>
//QMessageBox ( const QString & caption, const QString & text, Icon icon, int button0, int button1, int button2,
// QWidget * parent = 0, const char * name = 0, bool modal = TRUE, WFlags f = WStyle_DialogBorder )

//QMessageBox(Icon icon, const QString &title, const QString &text,
//StandardButtons buttons = NoButton, QWidget *parent = Q_NULLPTR,
//		Qt::WindowFlags flags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

class KMessageBox: public QMessageBox {
  Q_OBJECT
 public:
  static void error(QWidget* parent, const QString& text,
		    const QString &caption = "") {
    QMessageBox b(QMessageBox::Critical, caption, text, QMessageBox::Ok, parent);
    b.exec();
  }

  static int warningYesNo(QWidget* parent,
			   const QString& text,
			   const QString& s,
			   QObject *yes,
			   QObject *no,
			   const char *name) {
    QMessageBox b(QMessageBox::Warning, s, text,
				  QMessageBox::Yes | QMessageBox::No,
				  parent);
    return b.exec();
  }
			   
  static int warningYesNoCancel(QWidget* parent,
			   const QString& text,
			   const QString& s) {
    QMessageBox b(QMessageBox::Warning, s, text,
				  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, parent);
    return b.exec();
  }
};

#endif
