#ifndef NETACCESS_H
#define NETACCESS_H

#include "../kurl.h"

namespace KIO {
  class NetAccess {
  public:
    static bool download(const KURL& url, QString& str) {
      str = url.url();
      return true;
    }
    static void removeTempFile(const QString& name) {}
  };
}

#endif
