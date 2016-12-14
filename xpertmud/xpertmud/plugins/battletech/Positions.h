// -*- c++ -*-
#ifndef HEX_POSITIONS_H
#define HEX_POSITIONS_H

#include <cmath>
#ifdef WIN32
#pragma warning ( disable:4786 )
#ifndef M_PI
// there really seems to be no PI constant in the
// win32 math libraries...
#define M_PI 3.1415926535
#endif
#endif
#include <algorithm>
#include <qpoint.h>
#include <iostream>
using std::cout;
using std::endl;

// a Direction/Heading
// Basically just an angle, but provides classifiers for hex direction
class Direction {
public:
  enum DIR {
    N=0,
    NE=60,
    SE=120,
    S=180,
    SW=240,
    NW=300
  };
  explicit Direction(DIR d):dir(d),angle(static_cast<double>(d)) {}
  explicit Direction(double a) {
    setAngle(a);
  }
  void setAngle(double a) {
    angle=a;
    // TODO: Normalize!
    if (angle>= 30.0*M_PI/180.0 && angle < 90.0*M_PI/180.0) {
      dir=NE;
    } else if (angle>= 90.0*M_PI/180.0 && angle < 150.0*M_PI/180.0) {
      dir=SE;
    } else if (angle>= 150.0*M_PI/180.0 && angle < 210.0*M_PI/180.0) {
      dir=S;
    } else if (angle>= 210.0*M_PI/180.0 && angle < 270.0*M_PI/180.0) {
      dir=SW;
    } else if (angle>= 270.0*M_PI/180.0 && angle < 330.0*M_PI/180.0) {
      dir=NW;
    } else {
      dir=N;
    }
  }
  double getAngle() const { return angle; }
  void setDirection(DIR d) {
    dir=d;
    angle=static_cast<double>(dir);
  }
  DIR getDirection() const { return dir; }

protected:
  DIR dir;
  double angle;
};

// Position of a HEX, no subhex info
// Need negative values for drawing (!)
class HEXPos {
public:
  HEXPos(int x=0,int y=0, int z=0):
    x(x),y(y),z(z) {}

  HEXPos operator+(const HEXPos& pos) const {
    return HEXPos(x + pos.x, y + pos.y, z + pos.z);
  }

  // Movement operations
  // Hey, they seem to be correct (verified by <globbi>)
  // Ernst, moi compliment ;-)
  HEXPos N() const { 
    return HEXPos(x,(y-1),z); 
  }
  HEXPos S() const {
    return HEXPos(x,(y+1),z); 
  }
  HEXPos NE() const {
    return HEXPos((x+1),(odd()?y-1:y),z); 
  }
  HEXPos NW() const {
    return HEXPos((x-1),(odd()?y-1:y),z); 
  }
  HEXPos SE() const {
    return HEXPos((x+1),(odd()?y:y+1),z); 
  }
  HEXPos SW() const {
    return HEXPos((x-1),(odd()?y:y+1),z); 
  }
  HEXPos move(Direction::DIR d) {
    switch(d) {
    case Direction::N:return N();
    case Direction::NE:return NE();
    case Direction::SE:return SE();
    case Direction::S: return S();
    case Direction::SW:return SW();
    case Direction::NW:return NW();
    default: // Hae? compiler bug?
      return HEXPos();
    }
  }

  int getX() const { return x; }
  void setX(int nx) { x=nx; }
  int getZ() const { return z; }
  void setZ(int nz) { z= nz; }

  int getY() const { return y; }
  void setY(int ny) { y=ny; }

  bool operator==(const HEXPos & o) const {
    return 
      getX()==o.getX() &&
      getY()==o.getY() &&
      getZ()==o.getZ();
  }
protected:
  // odd x hexes are those hexes "higher" than even x hexes ... 
  inline bool odd() const  { return x % 2; }
  
  int x;
  int y;
  int z;
};

// Pure SubHex information
class SubPos {
public:
  SubPos(double angle=0.0, double dist=0.0):angle(angle),dist(dist) {}
  
  void setAngle(double a) { angle=a; }
  void setAngleDeg(double a) { angle=a*M_PI/180.0; }
  void setDistance(double d) { dist=d; }
  double getAngle() const { return angle; }
  double getAngleDeg() const { return angle*180.0/M_PI; }
  double getDistance() const { return dist; }
  // Get X,Y Position inside of HEX, 0,0 is at center of HEX
  // 1==Distance of two HEX centers
  double getSubX() const { return dist*cos(angle-M_PI/2.0);}
  double getSubY() const { return dist*sin(angle-M_PI/2.0);}

  const SubPos& operator+=(const SubPos& p) {
    double x = cos(angle)*dist + cos(p.angle)*p.dist;
    double y = sin(angle)*dist + sin(p.angle)*p.dist;

    angle = atan2(y, x);
    dist = sqrt(x*x + y*y);

    return *this;
  }
  bool operator==(const SubPos & o) const {
    return angle==o.angle && dist==o.dist;

  }

protected:
  double angle; // 0 == NORTH, PI/2 == EAST
  double dist; // 1 == sidelength of HEX
};

class SubHEXPos:public HEXPos, public SubPos {
public:
  SubHEXPos(const HEXPos& pos):
    HEXPos(pos),SubPos(0.0,0.0) {}

  SubHEXPos(int x=0, int y=0, int z=0,
	    double angle=0.0, double dist=0.0)
    :HEXPos(x,y,z),SubPos(angle,dist) {}

  
  double calcDistance(const SubHEXPos &pos) const;

  // TODO: Propagate operator+= from SubHexPos?
  // nope, we only need SubHEXPos += SubPos, which
  // is already there. We don't need SubHEXPos +=
  // SubHEXPos, for what?

  bool operator==(const SubHEXPos & o) const {
    return 
      HEXPos::operator==(o) &&
      SubPos::operator==(o);
  }
};


// hmmm, can't use any qt classes for this one
class Vector2D {
public:
  Vector2D(double x=0.0, double y=0.0): x(x), y(y) {}
  double x;
  double y;
};

/** HEX name documentation
-   [--- width ----] -
|       ********     |
|h     *        *    | cappedHeight
|e    *          *   |
|i   *            *  |
|g  *              * -
|h   *            *
|t    *          *
|      *        *
-       ********
    [----------]     cappedWidth
    [--][--][--][--] halfSide

Mathematical Description:

halfSide == width / 4.0 (see (A))
******
 *   *
 *   *
  *  * 
  *  * cappedHeight == dist(center, neighbourcenter) / 2.0 
   * *              == zoom / 2.0
   * *
    ** <- let the angle be alpha
     * 

tan(alpha) = halfSide / cappedHeight
=> halfSide = tan(alpha) * cappedHeight

because we've got a regular six edge
alpha = (360/6)/2 deg = 60/2 deg = 30 deg

=> halfSide = tan(30 deg) * cappedHeight
            = tan(30 deg) * zoom / 2
=> width = 4*halfSide = (2 * tan(30 deg)) * zoom

(A) Let side be the side length of a hex. Then
side*2 is the total width of the hex.

PROOF: Consider the equal sided triangles:
      [   c   ]
      *********    
     ***     ***   
    * * *   * * *  
   *  *  * *  *  * 
  *   *   *   *   *
  [ a ]       [ b ]
  [---- width ----]
It's easy to see that [a] and [b] together
form another equal sided triangle with the
same side length as [c]. Therefor 
[c] = [a]+[b], with [a] = [b] => [a] = [c]/2
=> width = [c]+[a]+[b] = [c]+[c]/2+[c]/2 = 2*[c].
qed.
*/

// <<<<<<<<<<<<<<<<< HERE

/// TODO: Manuel :)
/// WHUuaaaaaaa where's our robocup code ?????
inline double normAngle(double a) {
  while(a<0.0) {
    a+=2*M_PI;
  }
  while(a>=2*M_PI) {
    a-=2*M_PI;
  }
  return a;
}
///////////////////////////

/**
 * The normal hex layout as it is used
 * in the battletech server. You will need this
 * for calcualting things you send to the server,
 * for example tactical (angle, range).
 * The dist from the center of one hex to the
 * center of a neighbour hex is 1.0.
 */
class NormHexLayout {
public:
  static const double width;
  static const double height;

  static Vector2D hexToPoint(const HEXPos& hex) {
    double x = cappedWidth * hex.getX();
    double y = (double)(1-(abs(hex.getX()) % 2))*cappedHeight + 
      height*hex.getY();
    return Vector2D(x, y);
  }

  static Vector2D subPosToPoint(const SubHEXPos& pos) {
    // calculate the center of the hex in pos
    Vector2D leftUpper = hexToPoint(pos);
    Vector2D center(leftUpper.x + 2.0*halfSide,
		    leftUpper.y + cappedHeight);

    double x = center.x + pos.getSubX();
    double y = center.y + pos.getSubY();
    
    return Vector2D(x, y);
  }

  static SubPos difference(const SubHEXPos& src,
		    const SubHEXPos& dst) {
    Vector2D srcPoint = subPosToPoint(src);
    Vector2D dstPoint = subPosToPoint(dst);

    double dx = dstPoint.x-srcPoint.x;
    double dy = dstPoint.y-srcPoint.y;
    double alpha = normAngle(atan2(dy, dx) + M_PI/2.0);
    double dist = sqrt((dx*dx)+(dy*dy));

    return SubPos(alpha, dist);
  }

private:
  static const double cappedWidth; // width *3/4
  static const double cappedHeight; // height /2
  static const double halfSide; // width /4
};

// if we call pixelToHex, we still have
// an x/y modulo to the upper left border
// of the hex
class HEXPosModulo:public HEXPos {
public:
  HEXPosModulo(const HEXPos& pos, int modX, int modY):
    HEXPos(pos), modX(modX), modY(modY) {}

  int modX; // in pixel
  int modY; // in pixel
};

/**
 * An int-only hex layout. Needed for drawing
 * stuff.
 */
class HexLayout {
public:
  /**
   * zoom = dist(center to neighbour center) in pixel
   */
  HexLayout(double zoom=1.0, double deformation=1.0) {

    cappedHeight = static_cast<int>(zoom*deformation/2.0);
    height=cappedHeight*2;

    double bycath = zoom/2.0;
      
    // halfSide/bycath = tan(pi/6) = sqrt(1/3)
    halfSide = static_cast<int>(bycath*0.57735026918962573); 
    cappedWidth = 3 * halfSide;
    width=4 * halfSide;
  }

  /**
   * Definition of halfSide: see docs at top
   */
  HexLayout(int halfSide, double deformation=1.0):
    halfSide(halfSide)
  {
    cappedWidth = 3 * halfSide;
    width = 4 * halfSide;

    double bycath = (double)halfSide / 0.57735026918962573;
    double zoom = bycath*2.0;
    cappedHeight = static_cast<int>(zoom*deformation/2.0);
    height = cappedHeight*2;
  }

  bool operator!=(const HexLayout& l) {
    return (width != l.width ||
	    height != l.height);
  }

  HEXPosModulo pixelToHex(int x, int y) {
    unsigned int widthInX = x/cappedWidth;
    int modX = x % cappedWidth;

    int widthInXmod2 = widthInX % 2;
    int dwarf = y-(1-widthInXmod2)*cappedHeight;
    unsigned int heightInY = dwarf / (height);
    int modY = dwarf % height;

    return HEXPosModulo(HEXPos(widthInX, heightInY), modX, modY);
  }

  SubHEXPos pixelToSubHex(int x, int y) {
    // code redundance, but I think we need it
    // just take care ;-)
    unsigned int widthInX = x/cappedWidth;
    double xToCenter = (double)(x % cappedWidth)- 2.0*(double)halfSide;

    int widthInXmod2 = widthInX % 2;
    int dwarf = y-(1-widthInXmod2)*cappedHeight;
    unsigned int heightInY = dwarf / (height);
    double yToCenter = (double)(dwarf % height)-(double)cappedHeight;
    
    double dx = xToCenter * (NormHexLayout::width/(double)width);
    double dy = yToCenter / (double)height;
    double alpha = normAngle(atan2(dy, dx) + M_PI/2.0);
    double dist = sqrt(dx*dx+dy*dy);

    return SubHEXPos(widthInX, heightInY, 0, alpha, dist);
  }

  QPoint hexToPixel(const HEXPos& hex) {
    int x = cappedWidth * hex.getX();
    int y = (1-(abs(hex.getX()) % 2))*cappedHeight + height*hex.getY();
    return QPoint(x, y);
  }

  double getDeformizedDeg(double origAngle) {
    // TODO: Make static
    // the width of the normalized hex
    // width = 4 * 0.5 * tan(30 deg)
    static const double normWidth = 2.0 * 0.57735026918962573; 

    double x = cos(origAngle-M_PI/2.0) * ((double)width/normWidth);
    double y = sin(origAngle-M_PI/2.0) * (double)height;

    return (atan2(y, x)+M_PI/2)*180.0/M_PI;
  }

  QPoint subPosToPixel(const SubHEXPos& pos) {
    // calculate the center of the hex in pos
    QPoint leftUpper = hexToPixel(pos);
    QPoint center(leftUpper.x() + 2*halfSide,
		  leftUpper.y() + cappedHeight);

    // TODO: Make static
    // the width of the normalized hex
    // width = 4 * 0.5 * tan(30 deg)
    double normWidth = 2.0 * 0.57735026918962573; 

    // now calculate the new point by scaling
    // the differences up according to our int-valuez
    int x = center.x() + 
      (int)((double)pos.getSubX() * ((double)width/normWidth));
    int y = center.y() +
      (int)(pos.getSubY() * (double)height); // because normHeight == 1.0
    
    return QPoint(x, y);
  }

  int getWidth() const { return width; }
  int getHeight() const { return height; }

private:
  int cappedWidth;
  int width;
  int cappedHeight;
  int height;
  int halfSide;
};

#endif
