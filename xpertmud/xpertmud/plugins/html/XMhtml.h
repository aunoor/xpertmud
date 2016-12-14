// -*- c++ -*-
#ifndef XMUD_HTML_H
#define XMUD_HTML_H

#include <qhbox.h>
#include <qvariant.h>
class KHTMLPart;
class KURL;
namespace KParts {
    class URLArgs;
}
class XMHtml:public QHBox {
    Q_OBJECT
public:
    XMHtml(QWidget * parent, const char * name,const QStringList & args);

    // <Plugin Interface>
signals:
    void callback(int func, const QVariant & args, QVariant & result);

public slots:
     void slotFunctionCall(int func, const QVariant & args, QVariant & result);
    // </Plugin Interface>

    void slotCaptionChange(const QString &);
    void openURLRequest(const KURL &, const KParts::URLArgs &);

protected:
  KHTMLPart * htmlPart; 
};

#endif
