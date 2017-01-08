#ifndef XMUD_MENU_H
#define XMUD_MENU_H
#include <QMenuBar>
#include <QMouseEvent>

class XMMbackend;
class QWidget;

class XMMmenu:public QMenuBar {
  Q_OBJECT
public:
  XMMmenu(QWidget *parent = 0);
  ~XMMmenu();

public slots:
  void slotBackendSelected(XMMbackend *backend);

protected:
  QMenu *settings;
  QMenu *files;
  QMenu *edit;
  QMenu *zones;

protected slots:
  void slotSelectBackend();
  void slotConfigBackend();

signals:
  void emitConfigBackend(QWidget *parent);
  void emitSelectBackend(int backend_id);
}; 

#endif
