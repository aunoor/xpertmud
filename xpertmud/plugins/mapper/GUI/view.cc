#include "view.hh"
#include <qevent.h>

XMMmapView::XMMmapView(QWidget *parent):
        QGraphicsView(parent) {
}

XMMmapView::~XMMmapView() {
}

void XMMmapView::contentsMousePressEvent(QMouseEvent *e) {
/*
  QCanvasItemList list = canvas()->collisions(e->pos());
  if (list.isEmpty()) {
    emit emitAddRoom(e->x(), e->y());
  }
*/
}
