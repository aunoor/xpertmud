// -*- c++ -*-
#ifndef XMUD_MAPPERZONEBOX_H
#define XMUD_MAPPERZONEBOX_H

#include <qlistbox.h>

class MapperZoneBox:public QListBox {
  Q_OBJECT
public:
  MapperZoneBox(QWidget *parent, const char *name);
}; 

#endif
