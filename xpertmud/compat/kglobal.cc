#include "kglobal.h"
#include "kiconloader.h"

KGlobal* KGlobal::_instance = NULL;
KIconLoader* KGlobal::iconLoader() {
  return new KIconLoader();
}
