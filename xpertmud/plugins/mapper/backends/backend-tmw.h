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
    QMenu *menu;

private slots:
    void slotAutoGenerateMap();

};


#endif //XPERTMUD_BACKEND_TMW_CC_H
