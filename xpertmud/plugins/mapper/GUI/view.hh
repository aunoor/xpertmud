#ifndef XMUD_MAPVIEW_H
#define XMUD_MAPVIEW_H
#include <QGraphicsView>

class QMouseEvent;

class XMMmapView:public QGraphicsView {
  Q_OBJECT
public:
  XMMmapView(QWidget *parent);
  ~XMMmapView();

signals:
  void emitAddRoom(int x, int y);

protected:
  void contentsMousePressEvent(QMouseEvent *e);
};

#endif
