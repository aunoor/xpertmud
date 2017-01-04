// -*- c++ -*-
#ifndef XMUD_MAPPERMENU_H
#define XMUD_MAPPERMENU_H

#include <qmenubar.h>

class MapperMenu:public QMenuBar {
  Q_OBJECT
public:
  MapperMenu(QWidget *parent, const char *name);
}; 

#endif
