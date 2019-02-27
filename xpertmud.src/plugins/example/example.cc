#include "example.h"
#include <kgenericfactory.h>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include <qvariant.h>

XmudExample::XmudExample(QWidget * parent, const char * name,
			 const QStringList & /* unused: args*/):
    QWidget(parent) {
  setObjectName(name);
  QBoxLayout * layout=new QVBoxLayout(this);
  layout->setContentsMargins(0,0,0,0);
  myButton=new QPushButton(this);
  myButton->setText("Hallo");
  myButton->setFocusPolicy(Qt::NoFocus);
  layout->addWidget(myButton);
  connect(myButton, SIGNAL(clicked()), this, SLOT(slotButtonDown()));
  myLabel=new QLabel(this);
  layout->addWidget(myLabel);
}

void XmudExample::slotButtonDown() {
  QVariant result;
  emit callback(0,"Button was pressed",result);

  myLabel->setText(result.toString());
}

void XmudExample::slotFunctionCall(int func, const QVariant & args, QVariant & result) {
  switch(func) {
  case 0: // plugin->getText method
    result=myButton->text();
    break;
  case 1: // plugin->setText method
    myButton->setText(args.toString());
    break;
  case 2: // plugin->reset method;
    myLabel->setText("");
  default:
    result="Unknown function called";
  }
}


K_EXPORT_COMPONENT_FACTORY( xmud_example, KGenericFactory<XmudExample>( "xmud_example" ) )
