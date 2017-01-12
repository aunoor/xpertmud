#include "backend-tmw.h"
#include "main.hh"
#include "menu.hh"
#include "mobject.hh"

#include <QDebug>
#include <QMessageBox>

XMMbackend_TMW::XMMbackend_TMW(QObject *parent) : XMMAbstractBackend(parent) {
  m_parseMode = bpmIdle;

  m_waitLineStat = bplIdle;

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
  switchParseMode();
}

XMBackends XMMbackend_TMW::getId() {
  return XMBTMW;
}




void XMMbackend_TMW::switchParseMode() {
  XmudMapper *mapper = getParent();

  switch (m_parseMode) {
    case bpmIdle:
      if(!mapper) return;
      m_lines.clear();
      mapper->slotAddTrigger();
      mapper->slotSendLine("OUTPUTSUFFIX tmw_end_cmd\n");
      mapper->slotSendLine("OUTPUTPREFIX tmw_start_cmd\n");
      mapper->slotSendLine("@search type=room\n");
      m_parseMode = bpmRoomList;
      m_waitLineStat = bplWaitStartCmd;
      break;
    case bpmRoomList:
      m_parseMode = bpmRoomDetail;
      m_curRoomIdx = 0;
      getRoomDetails();
      break;
    case bpmRoomDetail:
      m_parseMode = bpmIdle;
      if (mapper) {
        mapper->slotDelTrigger();
        mapper->slotSendLine("OUTPUTPREFIX\n");
        mapper->slotSendLine("OUTPUTSUFFIX\n");
      }
      foreach (XMObject*obj, m_rooms) {
          emit newRoomSignal(obj);
      }
      qDeleteAll(m_rooms);
      m_rooms.clear();

    default:
      break;
  }
}

void XMMbackend_TMW::parseLine(QString line) {

  if (m_waitLineStat == bplIdle) return;

  QString rLine = line.trimmed();

  //qDebug() << "line:" << rLine;

  do {
    if (m_waitLineStat == bplWaitStartCmd) {
      if (rLine == QStringLiteral("tmw_start_cmd")) {
        m_waitLineStat = bplWaitEndCmd;
        return;
      }
    }

    if (m_waitLineStat == bplWaitEndCmd) {
      if (rLine == QStringLiteral("tmw_end_cmd")) {
        m_waitLineStat = bplIdle;
        break;
      }
      m_lines.append(rLine);
      return;
    }

    return;
  }while(0);


  if (m_parseMode==bpmRoomList) parseRoomList();
  if (m_parseMode==bpmRoomDetail) parseRoomDetails();
}

void XMMbackend_TMW::parseRoomList() {
  foreach (QString line, m_lines) {
      QString rLine = line.trimmed();
      QRegExp re("\\(#\\d+:.+\\)");

      int idIdx = rLine.indexOf(re);
      if (idIdx==-1) continue; //skip line

      QString rName = rLine.left(idIdx);

      QString rNumber=re.cap(0);
      rNumber = rNumber.mid(1,rNumber.indexOf(':')-1);

      XMObject *newRoom = new XMObject(rName, XMTRoom, rNumber);
      m_rooms.append(newRoom);

      qDebug() << "Name:" << rName << "number:"<< rNumber;
  }

  switchParseMode();
}

void XMMbackend_TMW::parseRoomDetails() {

  foreach(QString line, m_lines) {
      //get Flags:
      if (line.contains("Flags: ")) {
        QString flags=line.mid(line.indexOf("Flags: ")+7);
        qDebug() << flags;
      }
  }

  //m_rooms.at(m_curRoomIdx).
  m_lines.clear();
  getRoomDetails();
}

void XMMbackend_TMW::getRoomDetails() {
  XmudMapper *mapper = getParent();

  if (m_curRoomIdx>=m_rooms.count()) {
    switchParseMode();
    return;
  }

  QString rIdx = m_rooms.at(m_curRoomIdx)->getID();
  m_waitLineStat = bplWaitStartCmd;
  mapper->slotSendLine(QStringLiteral("examine ")+rIdx+"\n");
  m_curRoomIdx++;
}
