#include "backend-tmw.h"
#include "main.hh"
#include "menu.hh"
#include "mobject.hh"

#include <QDebug>
#include <QMessageBox>

XMMbackend_TMW::XMMbackend_TMW(QObject *parent) : XMMAbstractBackend(parent) {
  m_parseState = bpsIdle;
  menu = new QMenu("XmTmwMenu");
  menu->setTitle("Operations");

  menu->addAction("Autogenerate map", this, SLOT(slotAutoGenerateMap()));

  XmudMapper * mudMapper = getParent();

  if (mudMapper==NULL) return;

  mudMapper->getMenuBar()->addMenu(menu);

}

XMMbackend_TMW::~XMMbackend_TMW() {
  delete menu;
  if(getParent()) {
    getParent()->slotDelTrigger();
  }
}

QString XMMbackend_TMW::getBackendName() {
  return QStringLiteral("TinyMU* backend");
}

void XMMbackend_TMW::slotAutoGenerateMap() {
  XmudMapper *mapper = getParent();
  if(!mapper) return;

  if (mapper->getMap() == NULL) {
    QMessageBox::information(mapper, "Auto Mapper GUI", "You need to create map first");
    return;
  }

  m_parseState = bpsWaitRoom;
  mapper->slotAddTrigger();
  mapper->slotSendLine("@search type=room\n");

}

XMBackends XMMbackend_TMW::getId() {
  return XMBTMW;
}

void XMMbackend_TMW::parseLine(QString line) {

  if (m_parseState==bpsIdle) return; //just skip

  if(m_parseState==bpsWaitRoom) { //wait for "ROOMS:" - start of rooms list
    if (line.trimmed()=="ROOMS:") {
      m_parseState = bpsWaitEnd;
    }
    return;
  }

  if(m_parseState==bpsWaitEnd) { //processing line as room item and wait for list end
    if (line.contains("Found:  Rooms...")) {
      //end of rooms list
      m_parseState = bpsIdle;
      return;
    }

    QString rLine = line.trimmed();
    QRegExp re("\\(#\\d+:.+\\)");

    int idIdx = rLine.indexOf(re);
    if (idIdx==-1) return; //skip line

    QString rName = rLine.left(idIdx);

    QString rNumber=re.cap(0);
    rNumber = rNumber.mid(1,rNumber.indexOf(':')-1);

    XMObject *newRoom = new XMObject(rName, XMTRoom, rNumber);

    emit newRoomSignal(newRoom);

    qDebug() << "Name:" << rName << "number:"<< rNumber;

    return;
  }


}
