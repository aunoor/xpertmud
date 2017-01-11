#ifndef XMUD_MENU_H
#define XMUD_MENU_H
#include <QMenuBar>
#include <QMouseEvent>

class XMMAbstractBackend;
class QWidget;

class XMMmenu:public QMenuBar {
  Q_OBJECT
public:
  XMMmenu(QWidget *parent = 0);
  ~XMMmenu();

public slots:
  void slotBackendSelected(XMMAbstractBackend *backend);

protected:
  QMenu *settings;
  QMenu *files;
  QMenu *edit;
  QMenu *zones;
  QMenu *backends;

protected slots:
  void slotSelectBackend();

signals:
  void emitConfigBackend(QWidget *parent);
  void emitSelectBackend(int backend_id);
}; 

#endif
