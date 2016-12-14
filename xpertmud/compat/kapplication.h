#ifndef KAPP_H
#define KAPP_H

#include <qapplication.h>
#include <qpixmap.h>
#include "kconfig.h"
#include "kurl.h"
#include "kglobal.h"
#include "kiconloader.h"

#define kapp KApplication::kApplication()

class KApplication: public QApplication {
  Q_OBJECT
 public:
  KApplication():QApplication(nothing, NULL) {
    app = this;
    std::cout << font().family() << std::endl;
  }

  static KApplication *kApplication() { return app; }

  KConfig *config() { return new KConfig(); }

  QPixmap miniIcon() { 
    return KGlobal::iconLoader()->loadIcon("appIcon", KIcon::Small);
  }

  void invokeMailer(const KURL& url) {}

  bool isRestored() const { return false; }

 private:
  static int nothing;
  static KApplication *app;
};

#endif
