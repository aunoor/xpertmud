#ifndef KSTDACTION_H
#define KSTDACTION_H

#include "kaction.h"
class KStdAction {
 public:
  static KRecentFilesAction* openRecent(QObject*parent, const char* slot,
		    QObject* collection, const QString& name="")
    {
      return new KRecentFilesAction();
    }

  static KAction* quit(QObject*parent, const char* slot,
		    QObject* collection, const char* name="quit")
    {
      return new KAction("Quit", 0, parent, slot, collection, name);
    }

  static KToggleAction* showToolbar(QObject*parent, const char* slot,
		    QObject* collection, const QString& name="show_toolbar")
    {
      return new KToggleAction("Show Toolbar", 0, parent, slot,
			       collection, name);
    }

  static KToggleAction* showStatusbar(QObject*parent, const char* slot,
		    QObject* collection, const QString& name="show_statusbar")
    {
      return new KToggleAction("Show Statusbar", 0, parent, slot,
			       collection, name);
    }

  static KAction* configureToolbars(QObject*parent, const char* slot,
		    QObject* collection, const QString& name="")
    {
      return new KAction();
    }

  static KAction* preferences(QObject*receiver, const char* slot,
		    QObject* collection, const QString& name="preferences")
    {
      return new KAction("Preferences", 0, receiver, slot,
			 collection, name);
    }

  static KAction* openNew(QObject* receiver, const char* slot,
			  QObject* collection) {
    return new KAction("New", 0, receiver, slot,
		       collection, "open_new");
  }
			  
  static KAction* open(QObject* receiver, const char* slot,
		       QObject* collection) {
    return new KAction("Open", 0, receiver, slot,
		       collection, "open");
  }

  static KAction* save(QObject* receiver, const char* slot,
		       QObject* collection) {
    return new KAction("Save", 0, receiver, slot,
		       collection, "save");
  }

  static KAction* saveAs(QObject* receiver, const char* slot,
		       QObject* collection) {
    return new KAction("Save As", 0, receiver, slot,
		       collection, "save_as");
  }
};

#endif
