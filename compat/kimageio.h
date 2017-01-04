// -*- c++ -*-
#ifndef KIMAGEIO_H
#define KIMAGEIO_H

#include <qstring.h>

class KImageIO {
public:
  static QString type(const QString& filename) {
    return "png";
  }

  static void registerFormats() {
  }

  static QString pattern() {
    return "*.png";
  }
};

#endif
