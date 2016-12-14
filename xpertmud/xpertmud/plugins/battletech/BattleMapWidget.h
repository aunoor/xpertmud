// -*- c++ -*-
#ifndef BATTLEMAPWIDGET_H
#define BATTLEMAPWIDGET_H

#include "battletech.h" // BattleMapView
#include <qwidget.h>
#include <kurl.h>

class QSlider;
class QLabel;
class QAction;
class QPopupMenu;
class BattleCore;

class BattleMapWidget: public QWidget {
  Q_OBJECT
public:
  BattleMapWidget(QWidget *parent, const char *name, const QStringList& args);
  virtual ~BattleMapWidget();

  // <Plugin Interface> ///////////////////////////////////////////////////////
signals:
  void callback(int func, const QVariant & args, QVariant & result);

public slots:
  void slotFunctionCall(int func, const QVariant & args, QVariant & result);
  // </Plugin Interface> //////////////////////////////////////////////////////

private slots:
  void tileLeftClicked(QPoint p, SubHEXPos hex);
  void tileRightPressed(QPoint p, SubHEXPos hex);
  void tileMiddlePressed(QPoint p, SubHEXPos hex);
  void tileDoubleClicked(QPoint p, SubHEXPos hex);
  void mechLeftClicked(QPoint p, const QString& mech); // TODO: also give SubHEXPos for IDF
  void mechRightPressed(QPoint p, const QString& mech);
  void mechMiddlePressed(QPoint p, const QString& mech);
  void mechDoubleClicked(QPoint p, const QString& mech);
  void zoomChanged(double zoom);

  void lockCurrentHex(int mode);
  void tactCurrentHex();
  void jumpCurrentHex();

  void lockCurrentMech();
  void scanCurrentMech();
  void followCurrentMech();
  void followSelf();
  void saveMap();
  void loadMap();
  void clearMap();
  void saveImage();

  void slotValueChanged(int);

  void slotShowBar(bool);
  void createMechMenu();
  void createTileMenu();
  void createLockHexMenu();
  void createSettingsMenu();
  void centeredChanged();

private:

  bool zoomLock;
  QPopupMenu *tilePopup;
  QPopupMenu *mechPopup;
  QPopupMenu *settingsPopup;
  QPopupMenu *lockHexPopup;
  QWidget *speedbar;
  QSlider *slider;
  QLabel *centered;
  BattleMapView *view;
  BattleCore * core;

  QAction *showBar;

  // temporary data the user selected
  SubHEXPos currentHex;
  QString currentMech;
};

#endif
