#include "battletech.h"
#include "HeavyStyle.h"
#include "EasyStyle.h"

#include <cassert>

#include <algorithm>
using std::max;
using std::min;

#include <cmath>
#include <qfile.h>
#include <kimageio.h>

const int BattleMapView::MIN_ZOOM = 1;
const int BattleMapView::MAX_ZOOM = 40;
const int BattleMapView::MIN_X = -35;
const int BattleMapView::MIN_Y = -20;

BattleMapView::BattleMapView(QWidget * parent, const char * name):
  QWidget(parent, name,
	  WResizeNoErase|
	  WStaticContents|
	  WRepaintNoErase|
	  WPaintClever
	  ),
  x(0), y(0),
  style(new EasyStyle()),
  core(BattleCore::getInstance()),
  zoom(50.0), deformation(1.0)
{
  buffer = new QPixmap();
  doubleBuffer = new QPixmap();

  followedMech = "";
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
			    QSizePolicy::Expanding));
 
  /* Seems to be ok all the time
     it's _not_ on Windows (QT 2.3) !!!
  if(testWFlags(WResizeNoErase) &&
     testWFlags(WRepaintNoErase)) {
    cout << "yep" << endl;
  } else {
    cout << "Widget flags wrong" << endl;
  }
  */

  layout = HexLayout(zoom, deformation);

  assert(core != NULL);
  connect(core, SIGNAL(queueMechInfoChange(const MechInfo&, const MechInfo&)),
	  this, SLOT(queueMechInfoChange(const MechInfo&, const MechInfo&)));

  connect(core, SIGNAL(flushDisplayChange()),
	  this, SLOT(flushDisplayChange()));

  connect(core, SIGNAL(queueMapChange(const HEXPos&, const HEXPos&)),
	  this, SLOT(queueMapChange(const HEXPos&, const HEXPos&)));

  connect(core, SIGNAL(repaintCliffings()),
	  this, SLOT(repaintCliffings()));

  /* Debug code... loading map
  QFile f("testmap");
  f.open(IO_ReadOnly);
  QTextStream t(&f);
  BattleMap *m = new BattleMap("testmap", t);
  core->addMap(m);
  core->setCurrentMap("testmap");
  */
  QPainter paint;
  paint.begin(this);
  QFontMetrics fm = paint.fontMetrics();

  // calculate number metrics
  QRect rect = fm.boundingRect("8888"); // take one too much as fallback
  textWidthNum = rect.width()+4;
  textHeightNum = rect.height()+4;

  // calculate mech text metrics
  rect = fm.boundingRect("XXX"); // take one too much as fallback
  textWidthMech = rect.width();
  textHeightMech = rect.height();
  paint.end();

  // +4 because of the cool things we'll paint...
  upperBorder = max(textHeightNum+4, textHeightMech+4);
  leftBorder = max(textWidthNum+4, textWidthMech+4);
  lowerBorder = 20;
  rightBorder = 30;

  buffer->resize(width(), height());
  updateInternal(); 

}

BattleMapView::~BattleMapView() {
  BattleCore::returnInstance();
  delete buffer;
  delete doubleBuffer;
  delete style;
}

void BattleMapView::switchStyle(const QString& stylename) {
  delete style;
  if (stylename=="Heavy") {
    style=new HeavyStyle();
  } else {
    style=new EasyStyle();
  }
  updateInternal(); 
}

void BattleMapView::setFollowedMech(const QString& mech) {
  if (mech=="**" && core->getOwnId() != "")
    followedMech=core->getOwnId();
  else
    followedMech = mech;
  centerOnMech();
}

void BattleMapView::centerOnMech() {
  // followedMech is only ** if we don't know our own
  // mech id yet. So we can't do anything than.
  // followedMech will be changed from ** to something
  // useful in queueMechInfoChange()
  if(followedMech != "" && followedMech != "**") {
    MechInfo mi = core->getMechInfo(followedMech);
    QPoint pix = layout.subPosToPixel(mi.getPos());
    int nx = pix.x() - width()/2;
    int ny = pix.y() - height()/2;
    scrollMap(nx-x, ny-y);
  }
}

void BattleMapView::paintEvent(QPaintEvent *pe) {
  //  setUpdatesEnabled( false );
  /*
  cout << "Got Paint Event: (" << pe->rect().topLeft().x()
       <<", " << pe->rect().topLeft().y() << "), " 
       << pe->rect().width() << ", " << pe->rect().height()
       << endl;*/

  // leave this in. this is baaaad. Have to check it.

  // I think the X-Server is allowed to erase rects at will,
  // so this seems to be allright

  // yea, this is all right, but not while:
  // - scrolling, when we just called update()
  // - resizing,
  // because of ResizeNoErase and RepaintNoErase flags...

  // so if this does only matter while scrolling, 
  // output it only while scrolling

  // but resizing too, so I'd have to implement
  // an extra var, which is not nice...
  // anyways, leave it this way, because
  // if it works for scrolling, it works for
  // resizing, too :-)
  //  if(scrolling && pe->erased()) {
  //  cout << "WHUAAAAAAAAAAAAA NOOOOOO!!!!!!" << endl;
  //}
  
  bitBlt(this, pe->rect().x(), pe->rect().y(),
	 buffer, pe->rect().x(), pe->rect().y(), 
	 pe->rect().width(), pe->rect().height(), Qt::CopyROP, true);
  
  //  setUpdatesEnabled( true );
}

void BattleMapView::updateInternal() {
  //  cout << "updating all" << endl;
  updateInternal(QRegion(QRect(QPoint(0, 0), size())));
}



void BattleMapView::updateInternal(const QRegion& area) {
  updateMap(buffer, area);
  updateNumbers(buffer, area);
  updateMechs(buffer, area);
}

void BattleMapView::updateMap(QPaintDevice* device, const QRegion& area) {
  // calculate where to check if we have to redraw...
  QRect bounding(area.boundingRect());
  HEXPos upperLeft  = layout.pixelToHex(x+bounding.topLeft().x(), 
					y+bounding.topLeft().y());

  HEXPos lowerRight = layout.pixelToHex(x+bounding.bottomRight().x(),
		 			y+bounding.bottomRight().y());

  MechInfo self=core->getMechInfo(core->getOwnId()); 
  // TODO: is there something like core->getOwnMechinfo? 
  // nope, not yet... do we need it?

  // we're only drawing what's really needed, so let's
  // be generous here ;-)
  for(int curY=upperLeft.getY()-1; curY<lowerRight.getY()+2; ++curY) {
    for(int curX=upperLeft.getX()-1; curX<lowerRight.getX()+1; ++curX) {

      QPoint pupperLeft = layout.hexToPixel(HEXPos(curX, curY));
      QRect paintArea = QRect(pupperLeft.x()-x, pupperLeft.y()-y,
			      layout.getWidth(), layout.getHeight());

      QRegion clipArea = area & paintArea;
      if(!clipArea.isEmpty()) {
	MapTile t = core->getMapTile(HEXPos(curX, curY));
	MapTile tNW = core->getMapTile(HEXPos(curX, curY).NW());
	MapTile tSW = core->getMapTile(HEXPos(curX, curY).SW());
	MapTile tN =  core->getMapTile(HEXPos(curX, curY).N());
	/* todo let the BattleStyle decide when to draw
	   borders and how thick the cliffs are according to map-tile size */
	style->paintTile(device, clipArea, pupperLeft.x()-x, pupperLeft.y()-y, // TODO: pass paintArea instead of those 4 values
			 layout.getWidth(), layout.getHeight(),
			 t, ((zoom <= 7.0) ? false : true),
			 ((zoom <= 26.0) ? 0 : 2),
			 self.isCliff(t,tN), self.isCliff(t,tNW), self.isCliff(t,tSW));
      }
    }
  }
}

void BattleMapView::updateMechs(QPaintDevice* device, const QRegion& area) {
  QSize mechMess = 
    style->mechMess(device, layout.getWidth(), layout.getHeight());

  for(BattleCore::mechIteratorT it = core->getMechBegin();
      it != core->getMechEnd(); ++it) {

    // show only somehow newer mechs
    if (it->second.visible()) {
      QRect paintArea;
      bool inside;
      mechPosition(it->second.getPos(), it->second.getId(),
		   paintArea, inside);

      QRegion clipArea = area & paintArea;
      if(!clipArea.isEmpty()) {
	if(inside) {
	  style->paintMech(device, clipArea, 
			   paintArea.x()+mechMess.width()/2, 
			   paintArea.y()+mechMess.height()/2, 
			   layout.getWidth(), layout.getHeight(),
			   layout.getDeformizedDeg(it->second.getHeading()),
			   it->second,
			   15.0 + 15.0*(zoom-MIN_ZOOM)/(MAX_ZOOM-MIN_ZOOM));
	} else {
	  style->paintMechName
	    (device, clipArea, paintArea.x(), paintArea.y(), it->second);
	}
      }
    }
  }
}

void BattleMapView::updateNumbers(QPaintDevice* device, const QRegion &area) {

  QColor inC(200,200,200);
  QColor borderC(10,10,10);
  QColor lightC(255,255,255);
  QColor darkC(150,150,150);
  QColor textC(0,0,0);

  QPainter paint;
  paint.begin(device);
  paint.setClipRegion(area);

  QRect bounding(area.boundingRect());
  HEXPos upperLeft  = layout.pixelToHex(x+bounding.topLeft().x(), 
					y+bounding.topLeft().y());
  
  HEXPos lowerRight = layout.pixelToHex(x+bounding.bottomRight().x(),
					y+bounding.bottomRight().y());
  
  int incX = (int)ceil((double)textWidthNum / 
		       (double)(layout.getWidth()*0.5));
  int startX = upperLeft.getX()-1 - (upperLeft.getX()-1)%incX;
  // we'll check later what we really have to draw, so
  // again let's be generous here
  for(int curX=startX; curX<lowerRight.getX()+1+incX; curX+=incX) {
    if(curX >= 0) {
      QPoint pupperLeft = layout.hexToPixel(HEXPos(curX, upperLeft.getY()));
      int fromX = pupperLeft.x()-x+layout.getWidth()/2-textWidthNum/2;
      int fromY = 0;
      int toWidth = textWidthNum;
      int toHeight = textHeightNum;
      // that should be the bounding box of what you plan to draw
      QRect bnd(fromX-1,fromY-2, toWidth+2, toHeight+5);
      // intersect it with the redraw region and BINGO we have
      // we have a home brewed clipping. nbot sure it is better, but
      // at least test it :)
      if(QRegion(bnd).intersect(area).isEmpty())
	continue;
      QPointArray bound(5);
      bound[0] = QPoint(fromX, fromY-1);
      bound[1] = QPoint(fromX, fromY+toHeight);
      bound[2] = QPoint(fromX+toWidth/2, fromY+toHeight+3);
      bound[3] = QPoint(fromX+toWidth, fromY+toHeight);
      bound[4] = QPoint(fromX+toWidth, fromY-1);
      
      // the numbers update at schorschs Laptop as if the brush is not set correctly 
      // but I cant see a flaw here.
      // maybe you ahve tzo set an option the draw the polygon filled, and its pure
      // luck its working. WIld guess but all I can think of
      paint.setBrush(inC);
      paint.setPen(borderC);
#if QT_VERSION<300
      paint.drawPolygon(bound);
#else
      paint.drawConvexPolygon(bound);
#endif
      
      paint.setPen(lightC);
      paint.drawLine(bound[0]+QPoint(1,0), bound[1]+QPoint(1,-1));
      paint.setPen(darkC);
      paint.drawLine(bound[1]+QPoint(1,-1), bound[2]+QPoint(0,-1));
      paint.drawLine(bound[2]+QPoint(0,-1), bound[3]+QPoint(-1,-1));
      paint.drawLine(bound[3]+QPoint(-1,-1), bound[4]+QPoint(-1,0));
      
      paint.setPen(textC);
      paint.drawText(fromX, fromY, toWidth+1, toHeight,
		     Qt::AlignHCenter, QString::number(curX));
    }
  }

  // just a try... shouldn't be so bad to performance
  paint.flush();

  int incY = (int)(ceil((double)textHeightNum / 
			((double)layout.getHeight()*0.5)));
  int startY = upperLeft.getY()-2 - (upperLeft.getY()-2)%incY;
  for(int curY=startY; curY<lowerRight.getY()+2+incY; curY+=incY) {
    if(curY >= 0) {
      QPoint pupperLeft = layout.hexToPixel(HEXPos(upperLeft.getX(), curY));
      int yInc = 0;
      if(abs(upperLeft.getX()) % 2 == 1)
	yInc = layout.getHeight()/2;
      int fromX = 0;
      int fromY = pupperLeft.y()-y+layout.getHeight()/4+yInc-textHeightNum/2;
      int toWidth = textWidthNum-4;
      int toHeight = textHeightNum;
      
      QPointArray bound(5);
      bound[0] = QPoint(fromX-1, fromY);
      bound[1] = QPoint(fromX+toWidth, fromY);
      bound[2] = QPoint(fromX+toWidth+3, fromY+toHeight/2);
      bound[3] = QPoint(fromX+toWidth, fromY+toHeight);
      bound[4] = QPoint(fromX-1, fromY+toHeight);
      paint.setBrush(inC);
      paint.setPen(borderC);
// diabling convex polygom for testing purposes
// wild guess mode on
//#if QT_VERSION<300
      paint.drawPolygon(bound);
//#else
//     paint.drawConvexPolygon(bound);
//#endif
      
      paint.setPen(lightC);
      paint.drawLine(bound[0]+QPoint(0,1), bound[1]+QPoint(-1,1));
      paint.drawLine(bound[1]+QPoint(-1,1), bound[2]+QPoint(-1,0));
      paint.setPen(darkC);
      paint.drawLine(bound[2]+QPoint(-1,0), bound[3]+QPoint(-1,-1));
      paint.drawLine(bound[3]+QPoint(-1,-1), bound[4]+QPoint(0,-1));
      
      paint.setPen(QColor(0, 0, 0));
      paint.drawText(fromX, fromY, toWidth, toHeight+1,
		     Qt::AlignVCenter, QString::number(curY));
    }
  }
  paint.end();
}

void BattleMapView::mousePressEvent(QMouseEvent* ev) {
  SubHEXPos pos = layout.pixelToSubHex(x+ev->x(), y+ev->y());
  if(ev->button() == Qt::LeftButton) {
    scrolling = true;
    hasScrolled = false;
    mouseX = ev->globalX();
    mouseY = ev->globalY();
  } else if(ev->button() == Qt::MidButton) {
    QString id = findMech(ev->pos());
    if(id == "")
      emit tileMiddlePressed(ev->pos(), pos);
    else 
      emit mechMiddlePressed(ev->pos(), id);
  }else if(ev->button() == Qt::RightButton) {
    QString id = findMech(ev->pos());
    if(id == "")
      emit tileRightPressed(ev->pos(), pos);
    else
      emit mechRightPressed(ev->pos(), id);
  }
}

void BattleMapView::mouseDoubleClickEvent(QMouseEvent* ev) {
  SubHEXPos pos = layout.pixelToSubHex(x+ev->x(), y+ev->y());
  QString id = findMech(ev->pos());
  if(id == "")
    emit tileDoubleClicked(ev->pos(), pos);
  else
    emit mechDoubleClicked(ev->pos(), id);
}

void BattleMapView::mouseMoveEvent(QMouseEvent* ev) {
  if(scrolling) {
    hasScrolled = true;
    int mx = ev->globalX();
    int my = ev->globalY();

    setFollowedMech("");
    scrollMap(mouseX-mx, mouseY-my);

    mouseX = mx;
    mouseY = my;
  }
}

void BattleMapView::scrollMap(int dx, int dy) {
  if(x+dx < MIN_X)
    dx = MIN_X-x;

  if(y+dy < MIN_Y)
    dy = MIN_Y-y;

  if(dx != 0 || dy != 0) {
    if(!displayQueue.isEmpty()) {
      updateMap(buffer, displayQueue);
      updateMechs(buffer, displayQueue);
      updateNumbers(buffer, displayQueue);
      // not sure, but when the drawing operations are queued until after the
      // blit(in which you dont use the painter)
      // it might get confused. That could explain some of the problems we encountered
      //      p.flush(displayQueue); 
      displayQueue = QRegion();
    }
    x += dx;
    y += dy;

    /*
    cout << "bitBlt (0, 0) from (" << dx << ", " << dy 
	 << ", " << width() << ", " << height() << ")" << endl;
    */
    doubleBuffer->resize(buffer->size());
    /*
    cout << "DefaultOpt: " << QPixmap::defaultOptimization() << endl;
    cout << "NormalOptim: " << QPixmap::NormalOptim << endl;
    */
    bitBlt(doubleBuffer, 0, 0,
    	   buffer, dx, dy, width(), height(), Qt::CopyROP, true);

    // switch buffers
    // cool variable swap code, just not sure if ^ is XOR... 
    // buffer ^= doubleBuffer;
    // doubleBuffer ^=buffer;
    // buffer ^= doubleBuffer;
    // this code could confuse progs like valgrind badly, so better stick with the normal one,
    // or use std::swap :)

    QPixmap *temp = buffer;
    buffer = doubleBuffer;
    doubleBuffer = temp;
    //    p.end();
    

    QRect redrawUpper;
    QRect redrawLower;
    QRect redrawLeft;
    QRect redrawRight;
    if(dy < 0) {
      redrawUpper = QRect(0, 0, width(), upperBorder-dy);
      redrawLower = QRect(0, height()-lowerBorder, 
			  width(), lowerBorder);
    } else {
      /* leave in for later debuggin purposes please (!)
      cout << "Height: " << height() << endl
	   << "Width : " << width() << endl
	   << "dy    : " << dy << endl
	   << "lowerBorder: " << lowerBorder << endl
	   << "upperBorder: " << upperBorder << endl
	   << "leftBorder: " << leftBorder << endl
	   << "rightBorder: " << upperBorder << endl;
      */
      redrawLower = QRect(0, height()-lowerBorder-dy, 
			  width(), dy+lowerBorder);
      redrawUpper = QRect(0, 0, width(), upperBorder); 
    } 
    if(dx < 0) {
      redrawLeft = QRect(0, 0, leftBorder-dx, height());
      redrawRight = QRect(width()-rightBorder, 0,
			  rightBorder, height());
    } else { 
      redrawRight = QRect(width()-dx-rightBorder, 0,
			   dx+rightBorder, height());
      redrawLeft = QRect(0, 0, leftBorder, height()); 
    }
    QRegion upRegion(redrawUpper);
    upRegion +=redrawLower;
    upRegion += redrawLeft;
    upRegion += redrawRight;
    updateInternal(upRegion);

    // why is this shit so _damn_ fast ????
    // why is update() bad??? Is it erasing in spite of
    // our widget flags??? yes, it is
    // holy shit, then we have to stick to repaint
    // we'll have to see what it does in linux,
    // perhaps there update() is possible...
    // nope, in linux update() is also much, much
    // slower than repaint(false)...
    // I think we have to stick with repaint
    repaint(false);
  }
}

void BattleMapView::saveImage(const KURL & fn, const HEXPos & tL, const HEXPos & bR) {
  QString type=KImageIO::type(fn.fileName());
  QString file=fn.directory()+'/'+fn.fileName();
  int saved_x=x;
  int saved_y=y;
  QPoint beginArea=layout.hexToPixel(tL);
  x=beginArea.x();
  y=beginArea.y();
  QPoint areaEnd = layout.hexToPixel(HEXPos(bR.getX()+1,bR.getY()+2));
  QSize areaSize=QRect(beginArea,areaEnd).size();
  { // limit lifetime of qpixmap
    QPixmap savepix(areaSize,-1,QPixmap::MemoryOptim);
    QRect gcc_bug_preventer(QPoint(0,0),areaSize);
    QRegion area(gcc_bug_preventer);
    updateMap(&savepix, area);
    updateNumbers(&savepix, area);
    // No mechs in saved image
    //    updateMechs(&savepix, area);
    savepix.save(file,type);
  }
  x=saved_x;
  y=saved_y;
}

void BattleMapView::mouseReleaseEvent(QMouseEvent* ev) {
  scrolling = false;
  if(!hasScrolled) {
    SubHEXPos pos = layout.pixelToSubHex(x+ev->x(), y+ev->y());
    if(ev->button() == Qt::LeftButton) {
    QString id = findMech(ev->pos());
    if(id == "")
      emit tileLeftClicked(ev->pos(), pos);
    else
      emit mechLeftClicked(ev->pos(), id);
    }
  }
}

void BattleMapView::wheelEvent(QWheelEvent* ev) {
  double newZoom = zoom;
  newZoom += ev->delta()/120;
  //  newZoom = newZoom * pow(1.12, (double)ev->delta()/120.0);

  if(newZoom < (double)MIN_ZOOM) newZoom = (double)MIN_ZOOM;
  if(newZoom > (double)MAX_ZOOM) newZoom = (double)MAX_ZOOM;

  if(zoom != newZoom) {
    setZoomInternal(newZoom, ev->x(), ev->y());
  }
}

void BattleMapView::setZoomInternal(double newZoom, int midx, int midy) {
  zoom = newZoom;

  HEXPosModulo pos = layout.pixelToHex(x+midx, y+midy);
  
  HexLayout oldLayout = layout;
  layout = HexLayout((int)zoom, deformation);
  
  if(layout != oldLayout) {
    QPoint npix = layout.hexToPixel(pos);
    x = npix.x() + pos.modX*layout.getWidth()/oldLayout.getWidth() -
      midx;
    if(x<MIN_X) x = MIN_X;
    y = npix.y() + pos.modY*layout.getHeight()/oldLayout.getHeight() -
      midy;
    if(y<MIN_Y) y = MIN_Y;
    
    updateInternal();
    displayQueue = QRegion();
    update();
    //    repaint(false);
    
    emit zoomChanged(zoom);
  }
}

void BattleMapView::resizeEvent(QResizeEvent* ev) {
  buffer->resize(ev->size());

  // TOOD: optimize by only updating new areas in buffer, this is awfully slow
  updateInternal();
  /*
  updateInternal(QRect(QPoint(ev->oldSize().width(), 0),
		       QPoint(ev->size().width(),
			      ev->oldSize().height())));
  updateInternal(QRect(QPoint(0, ev->oldSize().height()),
		       QPoint(ev->size().width(),
			      ev->size().height())));
  update(QRect(QPoint(ev->oldSize().width(), 0),
	       QPoint(ev->size().width(),
		      ev->oldSize().height())));
  update(QRect(QPoint(0, ev->oldSize().height()),
	       QPoint(ev->size().width(),
		      ev->size().height())));
  */
  QWidget::resizeEvent(ev);
}


void BattleMapView::queueMechInfoChange(const MechInfo& oldInfo,
					const MechInfo& newInfo) {
  // if followedMech = "**" it could be we didn't know
  // until now, what the real mech id is
  if(followedMech == "**" && core->getOwnId() != "") {
    followedMech = core->getOwnId();
  }
  if ((!oldInfo.hasPos() || !(oldInfo.getPos() /* workaround for gcc bug */== newInfo.getPos())) ||
      (!oldInfo.hasHeading() || (oldInfo.getHeading() != newInfo.getHeading())) ||
      (oldInfo.visible() != newInfo.visible())) {
    
    QRect paintAreaOld;
    bool dummy;
    mechPosition(oldInfo.getPos(), oldInfo.getId(),
		 paintAreaOld, dummy);
    QRect paintAreaNew;
    mechPosition(newInfo.getPos(), newInfo.getId(),
		 paintAreaNew, dummy);
    
    displayQueue += QRegion(paintAreaOld & rect());
    displayQueue += QRegion(paintAreaNew & rect());
  }
}

void BattleMapView::flushDisplayChange() {
  centerOnMech(); // clears displayQueue if scroll changed
  updateInternal(displayQueue);
  update(displayQueue.boundingRect());
  displayQueue=QRegion();
}


void BattleMapView::queueMapChange(const HEXPos & from, const HEXPos& to) {
  QPoint leftUpper = layout.hexToPixel(from);
  QPoint rightLower = layout.hexToPixel(to);
  if(to.getX() > from.getX()) {
    leftUpper.setY(min(leftUpper.y(), 
		       layout.hexToPixel(from+HEXPos(1, 0)).y()));
    rightLower.setY(max(rightLower.y(),
		       layout.hexToPixel(to+HEXPos(-1, 0)).y()));
  }
  rightLower +=	QPoint(layout.getWidth(),layout.getHeight());
  
  leftUpper-=QPoint(x,y); //(leftUpper.x-x,leftUpper.y-y);
  rightLower-=QPoint(x,y);
  //  QPoint rl=rightL(rightLower.x-x,rightLower.y-y);

  QRect boundingRect(leftUpper,rightLower);
  // Only queue the part visible 
  QRect visibleArea = boundingRect & rect();
  
  if (!visibleArea.isEmpty()) 
    displayQueue += QRegion(visibleArea);
}

QString BattleMapView::findMech(QPoint p) {
// QSize mechMess = style->mechMess(layout.getWidth(), layout.getHeight());
  for(BattleCore::mechIteratorT it = core->getMechBegin();
      it != core->getMechEnd(); ++it) {
    // can't click on mechs that are not shown :-P
    if (it->second.visible()) {
      QPoint pos = layout.subPosToPixel(it->second.getPos());
      pos -= QPoint(x,y);
      if((pos.x() - p.x()) * (pos.x() - p.x()) +
	 (pos.y() - p.y()) * (pos.y() - p.y()) < 100) {
	return it->second.getId();
      }
    }
  }
  return "";
}


void BattleMapView::repaintCliffings() {
  updateInternal();
  update();
}

bool BattleMapView::lineIntersection(const QPoint& start1, const QPoint& end1,
				     const QPoint& start2, const QPoint& end2,
				     double& lambda1, double& lambda2) {
  // calculate the solution matrix
  double a11 = end1.x()-start1.x();
  double a12 = -(end2.x()-start2.x());
  double a21 = end1.y()-start1.y();
  double a22 = -(end2.y()-start2.y());

  double b1 = start2.x()-start1.x();
  double b2 = start2.y()-start1.y();

  double det = a11*a22 - a12*a21;
  if(det == 0) { return false; }
  
  lambda1 = (1/det) * (a22*b1 - a12*b2);
  lambda2 = (1/det) * (-a21*b1 + a11*b2);

  /*  double dx1 = end1.x()-start1.x();
  double dy1 = end1.y()-start1.y();
  double dx2 = end2.x()-start2.x();
  double dy2 = end2.y()-start2.y();*/
  if(lambda1 >= 0 && lambda1*lambda1 <= 1.0 &&
    lambda2 >= 0 && lambda2*lambda2 <= 1.0)
    return true;
  return false;
}


void BattleMapView::mechPosition(const SubHEXPos& hexPos, 
				 const QString& ref,
				 QRect& paintArea, bool& inside) {
  QSize mechMess = 
    style->mechMess(buffer, layout.getWidth(), layout.getHeight());
  QSize mechNameMess =
    style->mechNameMess(buffer, ref);

  QPoint pos = layout.subPosToPixel(hexPos);
  pos -= QPoint(x,y);
  paintArea = QRect(QPoint(pos.x()-mechMess.width()/2, 
			   pos.y()-mechMess.height()/2),
		    mechMess);
  QRegion clipArea = paintArea & QRect(QPoint(0, 0), size());
  if(!clipArea.isEmpty()) {
    inside = true;
    return;
  }
  inside = false;
  paintArea = QRect();
  double l1, l2;
  if(lineIntersection(QPoint(width()/2, height()/2), 
		      QPoint(pos.x(), pos.y()),
		      QPoint(0, 0), QPoint(width(), 0), l1, l2)) {
    // upper border
    paintArea = QRect(QPoint((int)(l2*width()), 0), mechNameMess);
  } else if(lineIntersection(QPoint(width()/2, height()/2), 
			     QPoint(pos.x(), pos.y()),
			     QPoint(0, 0), QPoint(0, height()), 
			     l1, l2)) {
    // left border
    paintArea = QRect(QPoint(0, (int)(l2*height())), mechNameMess);
  } else if(lineIntersection(QPoint(width()/2, height()/2), 
			     QPoint(pos.x(), pos.y()),
			     QPoint(width(), 0), 
			     QPoint(width(), height()), 
			     l1, l2)) {
    // right border
    paintArea = QRect(QPoint(width()-mechNameMess.width()-1, 
			     (int)(l2*height())), mechNameMess);
  } else if(lineIntersection(QPoint(width()/2, height()/2), 
			     QPoint(pos.x(), pos.y()),
			     QPoint(0, height()), 
			     QPoint(width(), height()), 
			     l1, l2)) {
    // lower border
    paintArea = QRect(QPoint((int)(l2*width()),
			     height()-mechNameMess.height()-1), mechNameMess);
  }
}



void BattleStyle::updateTileMask(int width,int height) {
   if (tileMask.width() != width || tileMask.height() != height) {
/*
  QMemArray<uchar> zerobits((int(width/8)+1)*height+1);
  zerobits.fill(0x00);
   
  tileMask=QBitmap(width, height, zerobits.data());

*/
    tileMask=QBitmap(width,height,true);
    
    QPainter pixPaint;
    pixPaint.begin(&tileMask);
//    pixPaint.begin(debuglabel);
////
    QPoint wUpper = QPoint(0, height/2-1);
    QPoint wLower = QPoint(0, height/2);
    QPoint nw = QPoint(width/4-1, 0);
    QPoint ne = QPoint(width*3/4-1, 0);
    QPoint sw = QPoint(width/4-1, height-1);
    QPoint se = QPoint(width*3/4-1, height-1);
    QPoint eUpper = QPoint(width-2, height/2-1);
    QPoint eLower = QPoint(width-2, height/2);
      
    QPointArray bound(8);
    bound[0]=wLower;
    bound[1]=wUpper;
    bound[2]=nw;
    bound[3]=ne;
    bound[4]=eUpper;
    bound[5]=eLower;
    bound[6]=se;
    bound[7]=sw;
      
    pixPaint.setBrush(Qt::color1);
    pixPaint.setPen(Qt::color1);


    pixPaint.drawConvexPolygon(bound);
//    pixPaint.drawPolygon(bound);
////

      pixPaint.end();
#ifdef DEBUG_MASK
    debuglabel->setPixmap(tileMask);
#endif
  }

}
