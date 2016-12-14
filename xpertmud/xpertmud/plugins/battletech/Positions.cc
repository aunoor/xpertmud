#include "Positions.h"


const double NormHexLayout::width = 2.0 * tan(M_PI/6);
const double NormHexLayout::height = 1.0;
const double NormHexLayout::cappedWidth = (3.0*NormHexLayout::width)/4.0;
const double NormHexLayout::cappedHeight = 0.5;
const double NormHexLayout::halfSide = NormHexLayout::width/4.0;

double SubHEXPos::calcDistance(const SubHEXPos &pos) const {

  SubPos d = NormHexLayout::difference(*this,pos);
  double a = (z - pos.getZ())/ 5.0;
  double b = d.getDistance();
  return sqrt(a*a + b*b);
}
