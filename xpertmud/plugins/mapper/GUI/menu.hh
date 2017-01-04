#ifndef XMUD_MENU_H
#define XMUD_MENU_H
#include <qmenubar.h>

class XMMbackend;
class QWidget;

class XMMmenu:public QMenuBar {
  Q_OBJECT
public:
  XMMmenu(QWidget *parent, const char *name);
  ~XMMmenu();

public slots:
  void slotBackendSelected(XMMbackend *backend);

protected:
  QPopupMenu *settings;
  QPopupMenu *files;
  QPopupMenu *edit;
  QPopupMenu *zones;

protected slots:
  void slotSelectBackend();
  void slotConfigBackend();

signals:
  void emitConfigBackend(QWidget *parent);
  void emitSelectBackend(int backend_id);
}; 

#endif
