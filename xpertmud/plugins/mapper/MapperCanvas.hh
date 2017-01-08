// -*- c++ -*-
#ifndef XMUD_MAPPERCANVAS_H
#define XMUD_MAPPERCANVAS_H

#include <QGraphicsView>


class MapperCanvas:public QGraphicsView {
  Q_OBJECT
public:
  MapperCanvas(QObject *parent, const char *name);
}; 

#endif
