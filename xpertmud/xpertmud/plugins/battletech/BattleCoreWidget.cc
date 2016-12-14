#include "BattleCoreWidget.h"
#include "BattleCore.h"

#include <qtextedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <klocale.h>

BattleCoreWidget::BattleCoreWidget(QWidget * parent, const char * name,
				   const QStringList & args):
  QWidget(parent,name), core(BattleCore::getInstance()) {

  QVBoxLayout* l =new QVBoxLayout(this);
  logViewer=new QTextEdit(this);
  logViewer->setReadOnly(true);
  l->addWidget(logViewer);
  QPushButton * cb=new QPushButton(i18n("Clear log"),this);
  l->addWidget(cb);
  connect(cb, SIGNAL(clicked()),
	  logViewer, SLOT(clear()));

  connect(core,SIGNAL(log(const QString &)),
	  this,SLOT(slotLog(const QString &)));

  connect(core,SIGNAL(send(const QString &)),
	  this,SLOT(slotSend(const QString &)));
  
  connect(core,SIGNAL(queueMechInfoChange(const MechInfo&, const MechInfo&)),
	  this,SLOT(mechInfoChange(const MechInfo&, const MechInfo&)));

}


BattleCoreWidget::~BattleCoreWidget() {
  BattleCore::returnInstance();
}

void BattleCoreWidget::slotFunctionCall(int func, const QVariant & arg, QVariant & result) {
  QVariant args(arg);
  if (func==0) {
    core->slotParseHUD(args.asString());
  } else if (func==1) {
    core->slotSetTacticalInterval(args.asString().toInt());
  } else if (func==2) {
    core->slotSetStatusInterval(args.asString().toInt());
  } else if (func==3) {
    result=core->getOwnId();
  } else if (func==4) { // getContacts
    QString acc;
    for (BattleCore::mechIteratorT it=core->getMechBegin();
	 it != core->getMechEnd(); ++it) {
      acc+=it->second.getId()+"|";
    }
    result=acc.left(acc.length()-1);
  } else if (func==5) {
    core->slotSetContactsInterval(args.asString().toInt());
  } else if (func==6) { // getContact
    MechInfo o = core->getMechInfo(args.asString());
    if(o.isValid())
      result=o.toString(core->getMechInfo(core->getOwnId()));
    else
      result.clear();
  } else if (func==7) { // setContact
    core->setMechInfo(args.asString());
  } else if (func==8) { // deleteContact
    core->deleteMechInfo(args.asString());
  } else if (func==9) { // setWeaponsInterval
    core->slotSetWeaponsInterval(args.asString().toInt());
  } else if (func==10) { // parse with text parser
    core->slotParseText(args.asString());
  } else if (func==11) {
    core->startSecondary();
  }
}

void BattleCoreWidget::slotLog(const QString & line) {
  logViewer->append(line+"\n");
  QVariant result;
  emit callback(2,QVariant(line),result);
}

void BattleCoreWidget::slotSend(const QString & cmd) {
  QVariant result;
  emit callback(0,QVariant(cmd),result);
}

void BattleCoreWidget::mechInfoChange(const MechInfo&, const MechInfo& mechInfo) {
  QVariant result;
  emit callback(1, mechInfo.toString(mechInfo), result);
}
