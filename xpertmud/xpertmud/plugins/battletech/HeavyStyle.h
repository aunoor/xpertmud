// -*- c++ -*-
#ifndef HEAVYSTYLE_H
#define HEAVYSTYLE_H

#include <qbitmap.h>
#include <qimage.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include "battletech.h"
#include "BattleCore.h"
#include <iostream>
using std::cout;
using std::endl;

#include <map>

class HeavyStyle: public BattleStyle {
  class MechCacheEntry {
  public:
    MechCacheEntry() {}
    MechCacheEntry(QPixmap* p, int width, int height, double heading):
      pixmap(p), width(width), height(height), heading(heading) {}
    QPixmap* pixmap;
    int width;
    int height;
    double heading;
  };

  // perhaps we'll want to enhance it later
  class TileCacheEntry {
  public:
    TileCacheEntry() {}
    TileCacheEntry(QPixmap* p, int width, int height):
      pixmap(p), width(width), height(height) {}
    QPixmap* pixmap;
    int width;
    int height;
  };

protected:
  std::map<QString, QImage> iMechs;  // the original. 

public:  
  // callSign -> last mech pixmap
  typedef std::map<QString, MechCacheEntry> mechCacheT;

  // "tile level width height" -> complete tile pixmap
  typedef std::map<QString, TileCacheEntry> tileCacheT;
public:
  HeavyStyle(double invScale = 1.0, bool drawHeading=true):
    invScale(invScale), drawHeading(drawHeading)
  {
    QString path;
    path = 
      KGlobal::dirs()->findResource("appdata", 
				    "artwork/battlemech_variant1.png");
    iMechs["B"].load(path);

    path = 
      KGlobal::dirs()->findResource("appdata", 
				    "artwork/heli.png");
    iMechs["V"].load(path);

    path = 
      KGlobal::dirs()->findResource("appdata", 
				    "artwork/installation_variant1.png");
    iMechs["U"].load(path);

    path = 
      KGlobal::dirs()->findResource("appdata", 
				    "artwork/tanks-wheelz.png");
    iMechs["TW"].load(path);

    path = 
      KGlobal::dirs()->findResource("appdata", 
				    "artwork/tanks-tracks.png");
    iMechs["TT"].load(path);
    
    path = 
      KGlobal::dirs()->findResource("appdata", 
				    "artwork/tanks-hoover.png");
    iMechs["TH"].load(path);

    path = 
      KGlobal::dirs()->findResource("appdata", 
				    "artwork/tanks-turrent.png");
    iMechs["TU"].load(path);

    path = 
      KGlobal::dirs()->findResource("appdata", 
				    "artwork/tanks-body.png");
    iMechs["TB"].load(path);

    path = 
      KGlobal::dirs()->findResource("appdata", 
				    "artwork/heavy_wood_variant1.png");
    iHeavyWood = QImage(path);
    path = 
      KGlobal::dirs()->findResource("appdata", 
				    "artwork/light_wood_variant1.png");
    iLightWood = QImage(path);

    path = 
      KGlobal::dirs()->findResource("appdata", 
				    "artwork/fire.png");
    iFire = QImage(path);

    path = 
      KGlobal::dirs()->findResource("appdata", 
				    "artwork/rubble.png");
    iRough = QImage(path);

    for (unsigned int i=0; i<64; ++i) {
      QString num=QString::number(i);
      if (num.length()==1)
        num="0"+num;
      path = 
        KGlobal::dirs()->findResource("appdata", 
	  			    QString("artwork/buildings/building%1.png").arg(num));
      iBuilding[i] = QImage(path);
    }

    for (unsigned int i=0; i<64; ++i) {
      QString num=QString::number(i);
      if (num.length()==1)
        num="0"+num;
      path = 
        KGlobal::dirs()->findResource("appdata", 
	  			    QString("artwork/roads/road%1.png").arg(num));
      iRoad[i] = QImage(path);
    }

    for (unsigned int i=0; i<64; ++i) {
      QString num=QString::number(i);
      if (num.length()==1)
        num="0"+num;
      path = 
        KGlobal::dirs()->findResource("appdata", 
	  			    QString("artwork/bridges/bridge%1.png").arg(num));
      iBridge[i] = QImage(path);
    }

    for (unsigned int i=0; i<64; ++i) {
      QString num=QString::number(i);
      if (num.length()==1)
        num="0"+num;
      path = 
        KGlobal::dirs()->findResource("appdata", 
	  			    QString("artwork/bridges/bridgedestroyed%1.png").arg(num));
      iBrokenBridge[i] = QImage(path);
    }

    for (unsigned int i=0; i<64; ++i) {
      QString num=QString::number(i);
      if (num.length()==1)
        num="0"+num;
      path = 
        KGlobal::dirs()->findResource("appdata", 
	  			    QString("artwork/walls/wall%1.png").arg(num));
      iWall[i] = QImage(path);
    }
 }

  virtual ~HeavyStyle() {
    // TODO: Cleanup those caches.
  }

  void setDrawHeading(bool set) { drawHeading = set; }
  bool getDrawHeading() { return drawHeading; }
  void setInverseScale(double scale) { invScale = scale; }
  double getInverseScale() { return invScale; }

  QSize mechMess(QPaintDevice* /*device*/, int width, int height) {
    // we'll need device for font size calculation,
    // but still a todo ;-)
    // TODO: Correct calculation... tricky
    // TODO: don't use the "B" image here, save maxsize while loading them.
    // (for larger images, like DropShips)
    QImage &img = iMechs["B"];
    int mWidth = img.width()*width/100 + 70;
    int mHeight = img.height()*height/100 + 70;
    return QSize(mWidth, mHeight);
  }

  QSize mechNameMess(QPaintDevice* device, const QString& ) {
    // we do it independent of the mech ref, but
    // other styles could want to use this
    QPainter paint;
    paint.begin(device);

    // TODO: set some nice font...
    QFontMetrics fm = paint.fontMetrics();
    QRect rect = fm.boundingRect("WW");

    return rect.size() + QSize(5, 5);
  }

  void paintMech(QPaintDevice* device, const QRegion& clipArea,
		 int x, int y, int width, int height,
		 double heading, const MechInfo& mechInfo,
		 double headingLength) {

    // TODO: add code to display rottorso and fliparms

    QString rtype;
    switch(mechInfo.getType()) {
    case 'Q':
    case 'B':
      rtype = "B";
      break;
    case 'V':
    case 'F':
    case 'A':
    case 'D':
      rtype = "V";
      break;
    case 'H':
      rtype = "H";
      break;
    case 'T':
      rtype = "T";
      break;
    case 'W':
      rtype = "W";
      break;
    case 'U':
    case 'I':
    case 'i':
      rtype = "U";
      break;
    default:
      rtype = "B";
    }
    int mWidth = (int)(iMechs["B"].width() * 
      (invScale * (double)width/100.0 + 
       (1-invScale)*(1-(double)width/100.0)));
    int mHeight = (int)(iMechs["B"].height() * 
      (invScale * (double)height/100.0 + 
       (1-invScale)*(1-(double)height/100.0)));
    mechCacheT::iterator mc = mechCache.find(mechInfo.getId());
    if(mc == mechCache.end() ||
       mc->second.width != mWidth || 
       mc->second.height != mHeight ||
       mc->second.heading != heading) {
      // generate new cache entry
      if(mc != mechCache.end())
	delete mc->second.pixmap;
      else {
	mc = mechCache.insert
	  (mechCacheT::value_type(mechInfo.getId(), MechCacheEntry())).first;
      }
      // and paint
      mc->second.pixmap = new QPixmap(mWidth, mHeight);
      mc->second.pixmap->setMask(QBitmap(mWidth, mHeight, true));

      mc->second.width = mWidth;
      mc->second.height = mHeight;
      mc->second.heading = heading;
      QPixmap scaled(mWidth, mHeight);
      scaled.setMask(QBitmap(mWidth, mHeight, true));
      if(rtype == "B" || rtype == "V" || rtype == "U") {
	scaled.convertFromImage
	  (iMechs[rtype].smoothScale(mWidth, mHeight));
      } else {
	QPixmap under;
	under.convertFromImage
	  (iMechs["T"+rtype].smoothScale(mWidth, mHeight));
	QPixmap body;
	body.convertFromImage
	  (iMechs["TB"].smoothScale(mWidth, mHeight));
	QPixmap scaledTurrent;
	scaledTurrent.convertFromImage
	  (iMechs["TU"].smoothScale(mWidth, mHeight));
	
	QPainter p2;
	p2.begin(scaled.mask());
	p2.setRasterOp(Qt::OrROP);

	// draw body alphamap
	p2.translate(mWidth/2, mHeight/2);
	p2.drawPixmap(QPoint(-mWidth/2, -mHeight/2), *under.mask());
	p2.drawPixmap(QPoint(-mWidth/2, -mHeight/2), *body.mask());

	// draw turned turrent alphamap
	if(mechInfo.hasTurretOffset())
	  p2.rotate(-(180.0-mechInfo.getTurretOffset()*180.0/M_PI));
	p2.drawPixmap(QPoint(-mWidth/2, -mHeight/2), *scaledTurrent.mask());

	p2.end();

	p2.begin(&scaled);

	// draw body
	p2.translate(mWidth/2, mHeight/2);
	p2.drawPixmap(QPoint(-mWidth/2, -mHeight/2), under);
	p2.drawPixmap(QPoint(-mWidth/2, -mHeight/2), body);
	
	// draw turned turrent
	// TODO: Check weather this turret behaviour really
	// is correct...
	// cout << "Turret: " << mechInfo.getTurretOffset()*180.0/M_PI << endl;
	if(mechInfo.hasTurretOffset()) 
	  p2.rotate(-(180.0-mechInfo.getTurretOffset()*180.0/M_PI));
	p2.drawPixmap(QPoint(-mWidth/2, -mHeight/2), scaledTurrent);

	p2.end();

      }
      QPainter pixPaint;
      // copy pic
      pixPaint.begin(mc->second.pixmap);
      pixPaint.translate(mWidth/2, mHeight/2);
      pixPaint.rotate(heading);
      pixPaint.drawPixmap(QPoint(-mWidth/2, -mHeight/2), scaled);
      pixPaint.end();

      // copy bitmask
      pixPaint.begin(mc->second.pixmap->mask());
      pixPaint.translate(mWidth/2, mHeight/2);
      pixPaint.rotate(heading);
      pixPaint.drawPixmap(QPoint(-mWidth/2, -mHeight/2), *scaled.mask());
      pixPaint.end();

    }

    paintMechName(device, clipArea, x+mWidth/2, y+mHeight/2, mechInfo);
    QPainter p;
    p.begin(device);
    p.setClipRegion(clipArea);
    if(drawHeading) {
      int xInc = (int)(headingLength * cos(heading*M_PI/180.0 - M_PI/2.0));
      int yInc = (int)(headingLength * sin(heading*M_PI/180.0 - M_PI/2.0));
      if(mechInfo.getId() == mechInfo.getId().upper() && mechInfo.getId() != "**") {
	p.setPen(QColor(210, 10, 15));
      } else {
	p.setPen(QColor(10, 210, 15));
      }
      p.drawLine(x, y, x+xInc, y+yInc);
    }
    p.drawPixmap(QPoint(x-mWidth/2, y-mHeight/2), 
    		 *mc->second.pixmap);
    p.end();
  }

  void paintMechName(QPaintDevice* device, const QRegion& clipArea,
		     int x, int y, 
		     const MechInfo &mechInfo) {
    // TODO: move the paint number from BattleMapView
    // into the style and make these style-global
    QColor inC(200,200,200);
    QColor borderC(10,10,10);
    QColor lightC(255,255,255);

    QRect rect = QRect(QPoint(x, y), mechNameMess(device, ""));

    QPainter p;
    p.begin(device);
    p.setClipRegion(clipArea);
    p.setPen(borderC);
    p.setBrush(inC);
    p.drawRect(rect);
    p.setPen(lightC);
    p.drawLine(rect.x()+1, rect.y()+1, rect.x()+1, rect.y()+rect.height()-2);
    p.drawLine(rect.x()+1, rect.y()+1, rect.x()+rect.width()-2, rect.y()+1);
    if(mechInfo.getId() == mechInfo.getId().upper() && 
       mechInfo.getId() != "**")
      p.setPen(QColor(70,10,5));
    else
      p.setPen(QColor(10,70,5));

    p.drawText(x, y, rect.width(), rect.height(),
	       Qt::AlignHCenter | Qt::AlignVCenter, mechInfo.getId());
    p.end();
    
  }


  void paintTile(QPaintDevice* device, const QRegion& clipArea,
		 int x, int y, int width, int height,
		 const MapTile& mapTile, 
		 bool borders,
		 int cliffs,
		 bool cliffN, bool cliffNW, bool cliffSW) {

    // skip this whole pixmap stuff when painting a tile smaller than a few pixels
      if (width <=3) {
	  QPainter p;
	  p.begin(device);
	  // draw the tile and the left upper and
	  // the left lower tile
	  p.fillRect(x,y,width,height,getColor(mapTile));

	  p.end();
		     

      } else { 
        updateTileMask(width,height);
    QString key;
    QTextStream keyStream(&key, IO_WriteOnly);
    // TODO: make this efficient (use an extra key class)
    keyStream << (int)mapTile.hasType()
	      << mapTile.getType()
	      << (int)mapTile.hasHeight()
	      << mapTile.getHeight()
              << mapTile.getSurroundings();

    tileCacheT::iterator tc = tileCache.find(key);
    if(tc == tileCache.end() ||
       tc->second.width != width ||
       tc->second.height != height) {
      //      cout << "Caching tile: \"" << key.latin1() << "\"" << endl;

      
      // generate new cache entry
      if(tc != tileCache.end()) {
	for(tileCacheT::iterator it = tileCache.begin();
	    it != tileCache.end();++it)
	  delete it->second.pixmap;
	tileCache.clear();
      }
      tc = tileCache.insert
	(tileCacheT::value_type(key, TileCacheEntry())).first;
      tc->second.pixmap = new QPixmap(width, height);
      tc->second.pixmap->setMask(tileMask);
      tc->second.width = width;
      tc->second.height = height;

      QPainter pixPaint;
      pixPaint.begin(tc->second.pixmap);
      // draw the tile and the left upper and
      // the left lower tile
/*      paintTile(pixPaint, -3*width/4, -height/2, width, height,
		nwMapTile, false); 
      paintTile(pixPaint, -3*width/4, height/2, width, height,
		swMapTile, false);  */
      paintTile(pixPaint, 0, 0, width, height, mapTile, borders); 
      pixPaint.end();
    }
    QRect blitArea(x, y, width, height); 
    if(blitArea.isValid()) {
      /* slow and painful
      QMemArray<QRect> rects = (clipArea & blitArea).rects();
      for(unsigned int i=0; i<rects.size(); ++i) {
	int inX = rects.at(i).x()-x;
	int inY = rects.at(i).y()-y;
	
	bitBlt(device, rects.at(i).x(), rects.at(i).y(),
	       tc->second.pixmap, inX, inY, 
	       rects.at(i).width(), rects.at(i).height(),
	       Qt::CopyROP, true);
      }
      */
     QRect rect = (clipArea & blitArea).boundingRect();
	int inX = rect.x()-x;
	int inY = rect.y()-y;
	
	bitBlt(device, rect.x(), rect.y(),
	       tc->second.pixmap, inX, inY, 
	       rect.width(), rect.height(),
	       Qt::CopyROP,false);

      // TODO: So here is the right position to indicate cliffings!
      // um, uh, what????
      QPoint wUpper = QPoint(blitArea.x(), blitArea.y()+height/2-1);
      QPoint wLower = QPoint(blitArea.x(), blitArea.y()+height/2);
      QPoint nw = QPoint(blitArea.x()+width/4-1, blitArea.y());
      QPoint ne = QPoint(blitArea.x()+width*3/4-1, blitArea.y());
      QPoint sw = QPoint(blitArea.x()+width/4-1, blitArea.y()+height-1);
      
      QPainter painter;
      painter.begin(device);
      //      painter.setClipRegion(clipArea);
      painter.setPen(QPen(QColor(250,50,0), cliffs));
      if (cliffNW)
	painter.drawLine(nw, wUpper);
      if (cliffN)
	painter.drawLine(nw, ne);
      if (cliffSW)
	painter.drawLine(sw, wLower);
      painter.end();
    }
      }
  }
  
private:
    QColor getColor(const MapTile & mapTile) {
    QColor c(124, 127, 233); // UNKNOWN
      
    // same basic color, different surface paintings
    if ((int)mapTile.getType()==MapTile::PLAINS)
      c = QColor(210,250,200);
    else if ((int)mapTile.getType()==MapTile::LIGHT_WOODS)
      c = QColor(70,200,70);
    else if ((int)mapTile.getType()==MapTile::HEAVY_WOODS)
      c = QColor(25,125,25);
    else if ((int)mapTile.getType()==MapTile::FIRE)
      c = QColor(200,50,30);
      
    // same basic color, different surface paintings
    else if ((int)mapTile.getType()==MapTile::ROAD)
      c = QColor(211,204,194);
    else if ((int)mapTile.getType()==MapTile::WALL)
      c = QColor(160,160,154);
    else if ((int)mapTile.getType()==MapTile::BUILDING)
      c = QColor(190,204,184);

    // alltogether different
    else if ((int)mapTile.getType()==MapTile::ICE)
      c = QColor(230,230,255);
    else if ((int)mapTile.getType()==MapTile::SNOW)
      c = QColor(250,250,255);
    else if ((int)mapTile.getType()==MapTile::MOUNTAIN)
      c = QColor(232,164,8);
    else if ((int)mapTile.getType()==MapTile::SMOKE)
      c = QColor(70,70,70);
    else if ((int)mapTile.getType()==MapTile::SMOKE2)
      c = QColor(50,50,50);
    else if ((int)mapTile.getType()==MapTile::WATER)
      c = QColor(100,100,230);
    else if ((int)mapTile.getType()==MapTile::BRIDGE)
      c = QColor(80,80,210);
     else if ((int)mapTile.getType()==MapTile::BROKEN_BRIDGE)
      c = QColor(60,60,170);
   else if ((int)mapTile.getType()==MapTile::ROUGH)
      c = QColor(232,213,8);
    else if ((int)mapTile.getType()==MapTile::DESERT)
      c = QColor(140,140,90);

    int tileHeight = mapTile.getHeight();
    
    // the higher the darker
    if((int)mapTile.getType()==MapTile::ICE ||
       (int)mapTile.getType()==MapTile::SNOW ||
       (int)mapTile.getType()==MapTile::MOUNTAIN ||
       (int)mapTile.getType()==MapTile::DESERT ||
       (int)mapTile.getType()==MapTile::ROUGH) {
      QRgb rgb = c.rgb();
      c.setRgb(qRgb((int)(qRed(rgb)*(1.0-(double)tileHeight/20.0)),
		    (int)(qGreen(rgb)*(1.0-(double)tileHeight/20.0)),
		    (int)(qBlue(rgb)*(1.0-(double)tileHeight/20.0))));
	       
    } 
    // the higher the lighter
    else if((int)mapTile.getType()==MapTile::PLAINS ||
	    (int)mapTile.getType()==MapTile::LIGHT_WOODS ||
	    (int)mapTile.getType()==MapTile::HEAVY_WOODS ||
	    (int)mapTile.getType()==MapTile::ROAD ||
	    (int)mapTile.getType()==MapTile::WALL ||
	    (int)mapTile.getType()==MapTile::BUILDING ||
	    (int)mapTile.getType()==MapTile::FIRE) {
      int tHeight=9-tileHeight;
      QRgb rgb = c.rgb();
      c.setRgb(qRgb((int)(qRed(rgb)*(1.0-(double)tHeight/20.0)),
		    (int)(qGreen(rgb)*(1.0-(double)tHeight/20.0)),
		    (int)(qBlue(rgb)*(1.0-(double)tHeight/20.0))));
    }
    // last but not least
    else if((int)mapTile.getType()==MapTile::WATER ||
	    (int)mapTile.getType()==MapTile::BRIDGE ||
	    (int)mapTile.getType()==MapTile::BROKEN_BRIDGE) {
      QRgb rgb = c.rgb();
      c.setRgb(qRgb((int)(qRed(rgb)*(1.0-(double)tileHeight/9.0)),
		    (int)(qGreen(rgb)*(1.0-(double)tileHeight/9.0)),
		    (int)(qBlue(rgb)*(1.0-(double)tileHeight/9.0))));
    }

    return c;

    }

  void paintTile(QPainter& pixPaint, int x, int y, int width, int height,
		 const MapTile& mapTile, bool borders) 
{ 
    QPoint wUpper = QPoint(x, y+height/2-1);
    QPoint wLower = QPoint(x, y+height/2);
    QPoint nw = QPoint(x+width/4-1, y);
    QPoint ne = QPoint(x+width*3/4-1, y);
    QPoint sw = QPoint(x+width/4-1, y+height-1);
    QPoint se = QPoint(x+width*3/4-1, y+height-1);
    QPoint eUpper = QPoint(x+width-2, y+height/2-1);
    QPoint eLower = QPoint(x+width-2, y+height/2);
      
    QPointArray bound(8);
    bound[0]=wLower;
    bound[1]=wUpper;
    bound[2]=nw;
    bound[3]=ne;
    bound[4]=eUpper;
    bound[5]=eLower;
    bound[6]=se;
    bound[7]=sw;
      
    QColor c=getColor(mapTile);
    pixPaint.setBrush(c);
    pixPaint.setPen(c);

#if QT_VERSION<300
    pixPaint.drawPolygon(bound);
#else
    pixPaint.drawConvexPolygon(bound);
#endif
    if (width>20 && height>20) {

    if((int)mapTile.getType()==MapTile::LIGHT_WOODS) {
      QPixmap scaled;
      scaled.convertFromImage(iLightWood.smoothScale(width, height));
      pixPaint.drawPixmap(QPoint(x, y), scaled);
    }
    else if((int)mapTile.getType()==MapTile::HEAVY_WOODS) {
      QPixmap scaled;
      scaled.convertFromImage(iHeavyWood.smoothScale(width, height));
      pixPaint.drawPixmap(QPoint(x, y), scaled);
    }
    else if((int)mapTile.getType()==MapTile::BUILDING) {
      QPixmap scaled;
      scaled.convertFromImage(iBuilding[mapTile.getSurroundings()].smoothScale(width, height));
      pixPaint.drawPixmap(QPoint(x, y), scaled);
    }
    else if((int)mapTile.getType()==MapTile::WALL) {
      QPixmap scaled;
      scaled.convertFromImage(iWall[mapTile.getSurroundings()].smoothScale(width, height));
      pixPaint.drawPixmap(QPoint(x, y), scaled);
    }
    else if((int)mapTile.getType()==MapTile::ROAD) {
      QPixmap scaled;
      scaled.convertFromImage(iRoad[mapTile.getSurroundings()].smoothScale(width, height));
      pixPaint.drawPixmap(QPoint(x, y), scaled);
    }
    else if((int)mapTile.getType()==MapTile::BRIDGE) {
      QPixmap scaled;
      scaled.convertFromImage(iBridge[mapTile.getSurroundings()].smoothScale(width, height));
      pixPaint.drawPixmap(QPoint(x, y), scaled);
    }
    else if((int)mapTile.getType()==MapTile::BROKEN_BRIDGE) {
      QPixmap scaled;
      scaled.convertFromImage(iBrokenBridge[mapTile.getSurroundings()].smoothScale(width, height));
      pixPaint.drawPixmap(QPoint(x, y), scaled);
    }
    else if((int)mapTile.getType()==MapTile::FIRE) {
      QPixmap scaled;
      scaled.convertFromImage(iFire.smoothScale(width, height));
      pixPaint.drawPixmap(QPoint(x, y), scaled);
    }
     else if((int)mapTile.getType()==MapTile::ROUGH) {
      QPixmap scaled;
      scaled.convertFromImage(iRough.smoothScale(width, height));
      pixPaint.drawPixmap(QPoint(x, y), scaled);
    }
  }

    pixPaint.setPen(QColor(0,0,0));
    if(borders) {
      pixPaint.drawLine(wUpper, nw);
      pixPaint.drawLine(nw, ne);
      pixPaint.drawLine(sw, wLower);
    }
    QString heightS;
    if(mapTile.hasHeight() && mapTile.getHeight() != 0)
      heightS += '0' + mapTile.getHeight();
    else if(!mapTile.hasHeight())
      heightS += '?';

    QString terrainAndHeight;
    if(mapTile.hasType() && mapTile.getType() != '.')
      terrainAndHeight += mapTile.getType();

    terrainAndHeight += heightS;

    QFontMetrics fm = pixPaint.fontMetrics();
    QRect tbound = fm.boundingRect("%8");
    
    if(tbound.width() <= width/2-2 && tbound.height() <= height-2) {
      pixPaint.drawText(x+width/4, y, x+width/2, y+height+1,
			Qt::AlignBottom | Qt::AlignRight, terrainAndHeight);
    }
  }
  
  double invScale;
  bool drawHeading;

  // the cached working version for a zoom level
  mechCacheT mechCache;

  // the cached working version for a zoom level
  tileCacheT tileCache;

  
  QImage iHeavyWood;  // the original. 
  QImage iLightWood;  // the original. 
  QImage iFire;
  QImage iRough;
  QImage iBuilding[64];  // the original.
  QImage iRoad[64];
  QImage iBridge[64];
  QImage iBrokenBridge[64];
  QImage iWall[64];
};

#endif
