#include "view.hh"
#include <qevent.h>

XMMmapView::XMMmapView(QWidget *parent, const char *name):
  QCanvasView(0, parent, name, 0) {
}

XMMmapView::~XMMmapView() {
}

void XMMmapView::contentsMousePressEvent(QMouseEvent *e) {
  QCanvasItemList list = canvas()->collisions(e->pos());
  if (list.isEmpty()) {
    emit emitAddRoom(e->x(), e->y());
  }
}
