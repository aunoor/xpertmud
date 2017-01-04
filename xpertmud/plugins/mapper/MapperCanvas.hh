// -*- c++ -*-
#ifndef XMUD_MAPPERCANVAS_H
#define XMUD_MAPPERCANVAS_H

#include <qcanvas.h>


class MapperCanvas:public QCanvas {
  Q_OBJECT
public:
  MapperCanvas(QObject *parent, const char *name);
}; 

#endif
