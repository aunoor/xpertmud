#ifndef KCMDLINEARGS_H
#define KCMDLINEARGS_H

#include "kaboutdata.h"
#include "kurl.h"
#include <qstring.h>

struct KCmdLineOptions {
  const char *name;
  const char *description;
  const char *def;
};

class KCmdLineArgs {
 public:
  static void init(int argc, char* argv[], KAboutData *about) {
  }
  static void addCmdLineOptions(KCmdLineOptions *options) {
  }
  static KCmdLineArgs* parsedArgs() {
    return new KCmdLineArgs();
  }

  QString arg (int id) { 
    return ""; 
  }
  int count() { return 0; }
};

#endif
