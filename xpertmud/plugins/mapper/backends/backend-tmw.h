#ifndef XPERTMUD_BACKEND_TMW_CC_H
#define XPERTMUD_BACKEND_TMW_CC_H

#include "backend.hh"

class QWidget;
class QMenu;

class XMMbackend_TMW:public XMMAbstractBackend {
Q_OBJECT
public:
    XMMbackend_TMW(QObject *parent = 0);
    ~XMMbackend_TMW();

    QString getBackendName() override;

    XMBackends getId() override;

    void parseLine(QString line) override;

private:
    enum BPMode {
        bpmIdle = 0,
        bpmRoomList,
        bpmRoomDetail
    };

    enum BPLStat {
        bplIdle=0,
        bplWaitStartCmd,
        bplWaitEndCmd
    };


    QMenu *menu;

    BPMode m_parseMode;
    BPLStat m_waitLineStat;

    QStringList m_lines;
    int m_curRoomIdx;
    QList<XMObject*> m_rooms;

    void switchParseMode();
    void parseRoomList();
    void parseRoomDetails();
    void getRoomDetails();
private slots:
    void slotAutoGenerateMap();

};


#endif //XPERTMUD_BACKEND_TMW_CC_H
