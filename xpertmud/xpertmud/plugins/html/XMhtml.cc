#include "XMhtml.h"

#include <kgenericfactory.h>
#include <qvbox.h>
#include <khtml_part.h>

XMHtml::XMHtml(QWidget * parent, const char * name,
			 const QStringList & /* unused: args*/):
    QHBox(parent,name) {
    htmlPart=new KHTMLPart(this,name);
    // TODO: setFocusPolicy!
    connect(htmlPart,SIGNAL(setWindowCaption(const QString &)),
	    this, SLOT(slotCaptionChange(const QString &)));

    connect(htmlPart->browserExtension(),
	    SIGNAL(openURLRequestDelayed(const KURL &, const KParts::URLArgs &)),
	    this, 
	    SLOT(openURLRequest(const KURL &, const KParts::URLArgs &)));
}

void XMHtml::openURLRequest(const KURL & url, const KParts::URLArgs &) {
    QVariant result;
    emit callback(1,url.url(),result);
}

void XMHtml::slotCaptionChange(const QString & caption) {
    QVariant result;
    emit callback(0,caption,result);
}

void XMHtml::slotFunctionCall(int func, const QVariant & args, QVariant & result) {
  switch(func) {
  case 0: // plugin->begin method
    htmlPart->begin(args.toString());
    break;
  case 1: // plugin->write method
    htmlPart->write(args.toString());
    break;
  case 2: // plugin->end method;
    htmlPart->end();
    break;
  case 3: // plugin->open method;
    result=htmlPart->openURL(args.toString());
    break;
  case 4: // plugin->zoom method;
    htmlPart->setZoomFactor(args.toInt());
    result=htmlPart->zoomFactor();
    break;
  case 5: // plugin->javascript method;
    result=htmlPart->executeScript(htmlPart->document(),args.toString());
    break;
  default:
    result="Unknown function called";
  }
}









K_EXPORT_COMPONENT_FACTORY( xmud_html, KGenericFactory<XMHtml>( "xmud_html" ) )
