#include "../MapperBackend.hh"
#include <gdbm.h>

MapperBackend::MapperBackend(QString *base = 0) {
  this->base = base;
}
