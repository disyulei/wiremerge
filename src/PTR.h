// ********************************************************************
//  Refer to K.D.Gourley and D.M. Green,
//  "A Polygon-to-Rectangle Conversion Algorithm", IEEE 1983
//
//    Author:    Bei Yu
//    Time:      Oct. 2011
// ********************************************************************

#ifndef _PTR_H_
#define _PTR_H_

#include "main.h"

class PTR
{
public:
  static bool Polygon2Rectangle(oaPointArray & points, std::vector<oaBox> & vBoxes);

private:
  static bool FindPkPlPm(const oaPointArray points, oaPoint& Pk, oaPoint& Pl, oaPoint& Pm);
  static void F(oaPointArray & points, int X, int Y);
  static void compress(oaPointArray & points, int difference = 0);
  static void compress_old(oaPointArray & points, int difference = 0);

  // debug functions
  static void print(const oaPointArray points, int min_x = 0, int min_y = 0);
  static int  getMinX(const oaPointArray points);
  static int  getMinY(const oaPointArray points);
  static bool findLine(const oaPointArray points);
};

#endif


