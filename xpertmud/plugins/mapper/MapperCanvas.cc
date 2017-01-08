#include "MapperCanvas.hh"

MapperCanvas::MapperCanvas(QObject *parent,
			   const char *name):
		QGraphicsView(parent) {
	setObjectName(name);
}
