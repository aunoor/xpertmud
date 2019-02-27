#include "MapperGUI.hh"
#include "MapperCanvas.hh"
#include "MapperMenu.hh"
#include "MapperZoneBox.hh"
#include <kgenericfactory.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qcanvas.h>
#include <qmenubar.h>

XmudMapper::XmudMapper(QWidget *parent, 
		       const char *name,
		       const QStringList & /* unused: args*/):
  QWidget(parent,name) {
  QBoxLayout *layout = new QHBoxLayout(this, 2);

  zonelist = new MapperZoneBox(this, "mapperZoneList");
  mapview = new QCanvasView (0, this, "mapperMapView", 0);
  mapcanvas = 0;
  menu = new MapperMenu(this, "mapperMenu");

  layout->addWidget(zonelist);
  layout->addWidget(mapview);
}

void XmudMapper::slotFunctionCall(int func, const QVariant & args, QVariant & result) {
  switch(func) {
  default:
    result="Unknown function called";
  }
}

K_EXPORT_COMPONENT_FACTORY( xmud_mapper, KGenericFactory<XmudMapper>( "xmud_mapper" ) );
