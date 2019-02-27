#ifndef XMUD_MAPVIEW_H
#define XMUD_MAPVIEW_H
#include <qcanvas.h>

class QMouseEvent;

class XMMmapView:public QCanvasView {
  Q_OBJECT
public:
  XMMmapView(QWidget *parent, const char *name);
  ~XMMmapView();

signals:
  void emitAddRoom(int x, int y);

protected:
  void contentsMousePressEvent(QMouseEvent *e);
};

#endif
