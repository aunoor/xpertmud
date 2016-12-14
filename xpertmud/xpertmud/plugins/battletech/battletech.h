// -*- c++ -*-
#ifndef BATTLETECH_H
#define BATTLETECH_H

#include <qwidget.h>
#include <qvariant.h>
#include <qpainter.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qbitmap.h>

#include "Positions.h"
#include "BattleCore.h"
class MechInfo;
class BattleStyle {
public:
  virtual ~BattleStyle() {}
  virtual void paintMech(QPaintDevice* device, const QRegion& clipArea,
			 int x, int y, int width, int height,
			 double heading, const MechInfo &,
			 double headingLength)=0;
  
  virtual void paintMechName(QPaintDevice* device, const QRegion& clipArea,
			     int x, int y, 
			     const MechInfo& )=0;

  virtual void paintTile(QPaintDevice* device, const QRegion& clipArea,
			 int x, int y, int width, int height,
			 const MapTile& mapTile, 
			 bool borders,
			 int cliffs,
			 bool cliffN, bool cliffNW,
			 bool cliffSW)=0;

  virtual QSize mechMess(QPaintDevice* device, int width, int height)=0;
  virtual QSize mechNameMess(QPaintDevice* device, const QString& ref)=0;

protected:
  void updateTileMask(int width,int height);
  QBitmap tileMask;

};

class BattleMapView: public QWidget {
  Q_OBJECT
public:
  BattleMapView(QWidget * parent, const char * name);
  ~BattleMapView();
  void switchStyle(const QString & name);
  double getMaxZoom() const { return MAX_ZOOM; }
  double getMinZoom() const { return MIN_ZOOM; }
  double getZoom() const { return zoom; }
  void setZoom(double zoom) { 
    if(zoom >= MIN_ZOOM && zoom <= MAX_ZOOM)
      setZoomInternal(zoom, width()/2, height()/2);
  }
  /*
   * Set to "" to reset.
   */
  void setFollowedMech(const QString& mech);
  QString getFollowedMech() { return followedMech; }
  void scrollMap(int dx, int dy);

  void saveImage(const KURL & fn, const HEXPos & tL, const HEXPos & bR);

protected:
  void centerOnMech();
  void paintEvent(QPaintEvent * pe);
  void mousePressEvent(QMouseEvent* ev);
  void mouseDoubleClickEvent(QMouseEvent* ev);
  void mouseMoveEvent(QMouseEvent* ev);
  void mouseReleaseEvent(QMouseEvent* ev);
  void wheelEvent(QWheelEvent* ev);
  void setZoomInternal(double zoom, int midx, int midy);
  void resizeEvent(QResizeEvent* ev);
  
  void updateInternal();
  //  void updateInternal(const QRect& area);
  void updateInternal(const QRegion& area);

  void updateMap(QPaintDevice* device, const QRegion& area);
  void updateMechs(QPaintDevice* device, const QRegion& area);
  void updateNumbers(QPaintDevice* device, const QRegion &area);

signals:
  void tileLeftClicked(QPoint p, SubHEXPos hex);
  void tileRightPressed(QPoint p, SubHEXPos hex);
  void tileMiddlePressed(QPoint p, SubHEXPos hex);
  void tileDoubleClicked(QPoint p, SubHEXPos hex);
  void mechLeftClicked(QPoint p, const QString& mech);
  void mechRightPressed(QPoint p, const QString& mech);
  void mechMiddlePressed(QPoint p, const QString& mech);
  void mechDoubleClicked(QPoint p, const QString& mech);
  void zoomChanged(double zoom);


private slots:

  void queueMechInfoChange(const MechInfo& oldInfo, const MechInfo& newInfo);
  void queueMapChange(const HEXPos &, const HEXPos &);
  void flushDisplayChange();
  void repaintCliffings();

private:
  // find the mech at the coordinates p on the widget.
  // returns "" if there's no mech
  QString findMech(QPoint p);

  bool lineIntersection(const QPoint& start1, const QPoint& end1,
			const QPoint& start2, const QPoint& end2,
			double& lambda1, double& lambda2);
  void mechPosition(const SubHEXPos& hexPos, 
		    const QString& ref,
		    QRect& paintArea, bool& inside);

  // x, y in the deformated pixelized (virtual) map
  // e.g. our left upper corner...
  // TODO: rename to something that doesn't possibly clash with QT internals
  int x;
  int y;
  QRegion displayQueue;

  HexLayout layout;
  BattleStyle *style;

  // the mech we center on, free if ""
  QString followedMech;

  int mouseX;
  int mouseY;
  bool scrolling;
  bool hasScrolled;

  // for nice looking drawing...
  QPixmap *buffer;
  // needed only for scrolling with older qt versions...
  // else we had to bitBlt ourself onto ourself, which is
  // not supported in all cases
  QPixmap *doubleBuffer;

  //  QPainter paint;
  int textWidthNum;
  int textHeightNum;
  int textWidthMech;
  int textHeightMech;
  int upperBorder;
  int leftBorder;
  int lowerBorder;
  int rightBorder;
  
  BattleCore * core;

  double zoom;
  double deformation;

  // Shouldn't they be stored in the style? Or would that be to slow?
  // no, method calles are not the problem ;-) 
  // good idea, TODO: Evaporate, put into style
  static const int MIN_ZOOM; // =10;
  static const int MAX_ZOOM; // =100;
  static const int MIN_X; // =-35;
  static const int MIN_Y; // =-20;
};

#endif
