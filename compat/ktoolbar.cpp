
#include "ktoolbar.h"

KToolBar::KToolBar(const QString& text, QMainWindow* parent,
         Qt::ToolBarArea dock,
         bool newline, const char *name):
        QToolBar(text, parent) {
    this->setObjectName(QString(name));
    parent->addToolBarBreak(dock);
    parent->addToolBar(dock, this);
}

KToolBar::KToolBar(QMainWindow* parent, const char *name, bool, bool):
QToolBar(parent) {
    this->setObjectName(QString(name));
    parent->addToolBar(this);
}

void KToolBar::insertWidget(int id, int, QWidget*w, int) {
    cout << "inserted... " << id << endl;
    cout << "hmmm" << (long)w << endl;
    id2widget.insert(id, w);
    cout << "done" << endl;
    this->addWidget(w);
}

void KToolBar::setItemAutoSized(int id, bool enable) {
    Id2WidgetMap::Iterator it = id2widget.find( id );
    QWidget *w = (( it == id2widget.end() ) ? 0 : (*it));
    if(w && enable) {
        cout << "yep!" << endl;
        //setStretchableWidget(w);
    }
}