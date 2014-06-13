#ifndef _B_LIB_PTR_H_
#define _B_LIB_PTR_H_

// ===================================================================
//    class       : PTR
//    author      : Bei Yu
//    last update : 06/2014
//
//  Refer to K.D.Gourley and D.M. Green,
//  "A Polygon-to-Rectangle Conversion Algorithm", IEEE 1983
//
// ===================================================================

#include "bLibPoint.h"
#include <iostream>
#include <algorithm>  // min()
#include <vector>
#include <climits>
#include <set>
#include <cstdio>

namespace bLib
{

//#define _DEBUG_PTR
class PTR
{
public:
  template<class Type>
  static bool polygon2Rect(std::vector<myPoint>& points, std::vector<Type> & vBoxes);

private:
  // operations on std::vector
  static void compress(std::vector<myPoint> & points, int difference = 5);
  static bool findLine(const std::vector<myPoint> points);
  static bool findPkPlPm(const std::vector<myPoint>&, myPoint&, myPoint&, myPoint&);
  static bool findVkVlVm(const std::vector<myPoint>&, myPoint&, myPoint&, myPoint&);
  static void F(std::vector<myPoint>& points, int X, int Y);
  static int  getHorRangeNext(const std::vector<myPoint>&,const int,const int,const int);
  static int  getVerRangeNext(const std::vector<myPoint>&,const int,const int,const int);
  static void print(const std::vector<myPoint> points, int min_x = 0, int min_y = 0);

  // operations on std::set
  static bool findPkPlPm(const std::set<myPoint>&, myPoint&, myPoint&, myPoint&);
  static bool findVkVlVm(const std::set<myPoint>&, myPoint&, myPoint&, myPoint&);
  static void F(std::set<myPoint>& points, int X, int Y);
  static int  getHorRangeNext(const std::set<myPoint>&,const int,const int,const int);
  static int  getVerRangeNext(const std::set<myPoint>&,const int,const int,const int);
  static void print(const std::set<myPoint> points, int min_x = 0, int min_y = 0);
};

// ============================================
//              inline functions
// ============================================
// Refer to [Courley+, IEEE'83]: "A Polygon-to-Rectangle Conversion Algorithm"
// NEW improvement!!
// meanwhile, reduce the slice rectangle
// Input  : points
// Output : boxes
template<class Type>
inline bool
PTR::polygon2Rect(std::vector<myPoint>& vpoints, std::vector<Type>& boxes)
//{{{
{
#ifdef _DEBUG_PTR
  std::cout << "debug| PTR::Polygon2Rectangle(), vpoints = " << std::endl;
  print(vpoints);
#endif

  // Step 1: check polygon
  compress(vpoints, 0);   // smooth polygon, i.e., reduce corner point number
  if (true == findLine(vpoints))
  {
    std::cout << "ERROR| PTR::polygon(), find three continuous points are on a line" << std::endl;
    return false;
  }

  // Step 2: iteratively remove box
  while (vpoints.size() > 0)
  {
    myPoint Pk, Pl, Pm;
    myPoint Vk, Vl, Vm; // similar to Pk,Pl,Pm, but scan vertical edges now

    if (false == findPkPlPm(vpoints, Pk, Pl, Pm))
    {
      //std::cout << "ERROR| PTR::Polygon2Rectangle(), can NOT find Pk, Pl, and Pm." << std::endl;
      return false;
    }
    if (false == findVkVlVm(vpoints, Vk, Vl, Vm))
    {
      //std::cout << "ERROR| PTR::Polygon2Rectangle(), can NOT find Vk, Vl, and Vm." << std::endl;
      return false;
    }

    // until now: two candidates:
    int slide_y = getHorRangeNext(vpoints, Pk.x(), Pl.x(), Pm.y()) - Pm.y();
    int slide_x = getVerRangeNext(vpoints, Vk.y(), Vl.y(), Vm.x()) - Vm.x();

    Type box;
    if (slide_y >= slide_x) box.set(Pk.x(), Pk.y(), Pl.x(), Pm.y());
    else                    box.set(Vk.x(), Vk.y(), Vm.x(), Vl.y());
    boxes.push_back(box);
    
    F(vpoints, box.x1(), box.y1());
    F(vpoints, box.x1(), box.y2());
    F(vpoints, box.x2(), box.y1());
    F(vpoints, box.x2(), box.y2());

    #ifdef _DEBUG_PTR
    printf ("DEBUG| add myBox (%d %d), (%d %d) into output vector<myBox>\n", box.x1(), box.y1(), box.x2(), box.y2());
    printf ("DEBUG| new points: \n");
    print(vpoints);
    printf ("\n");
    #endif
  } // while

  return true;
}
//}}}


//  Given points, find Pk, Pl and Pm
//  Pk: the leftmost of the lowest points
//  Pl: the next leftmost of the lowest points
//  Pm: 1) Xk <= Xm < Xl
//      2) Ym is lowest but Ym > Yk (Yk == Yl)
inline bool
PTR::findPkPlPm(const std::vector<myPoint>& points, myPoint& Pk, myPoint& Pl, myPoint& Pm)
//{{{
{
  if (points.size() < 4)
  {
  #ifdef _DEBUG_PTR
    printf ("DEBUG| points.size() = %d\n", (int)points.size());
  #endif
    return false;
  }

  int min_y = INT_MAX; 
  int min_x = INT_MAX; int next_x = INT_MAX;

  // first round, determine Pk, Pl
  for (int i=0; i<points.size(); i++)
  {
    int x = points[i].x(), y = points[i].y();
    if (y < min_y)
    {
      min_y = y;
      min_x = next_x = INT_MAX;
    }
    if (y > min_y) continue;

    // here: y == min_y
    if (x < min_x)
    {
      next_x = min_x;
      min_x = x;
    }
    else if (x > min_x && x < next_x)
    {
      next_x = x;
    }
  }
  Pk.set(min_x, min_y);
  Pl.set(next_x, min_y);

  // second round, determine Ym (next_y)
  int Ym = getHorRangeNext(points, Pk.x(), Pl.x(), min_y);

  // third round, determine Xm
  int Xm = INT_MAX;
  for (int i=0; i<points.size(); i++)
  {
    int x = points[i].x(), y = points[i].y();
    if (y != Ym) continue;
    if (Xm > x) Xm = x;
  }

  #ifdef _DEBUG_PTR
  if (Ym >= INT_MAX || Xm >= INT_MAX)
  {
    std::cout << "DEBUG| PTR::findPkPlPm(), output the points" << std::endl;
    for (int i=0; i<points.size(); i++)
    {
      std::cout << points[i].x() << ", " << points[i].y() << std::endl;
    } // for sitr
  }
  #endif
  if (Ym >= INT_MAX) return false;
  if (Xm >= INT_MAX) return false;

  Pm.set(Xm, Ym);
  return true;
}
//}}}

inline bool
PTR::findPkPlPm(const std::set<myPoint>& points, myPoint& Pk, myPoint& Pl, myPoint& Pm)
//{{{
{
  if (points.size() < 4)
  {
  #ifdef _DEBUG_PTR
    printf ("DEBUG| points.size() = %d\n", (int)points.size());
  #endif
    return false;
  }

  int min_y = INT_MAX; 
  int min_x = INT_MAX; int next_x = INT_MAX;

  // first round, determine Pk, Pl
  for (std::set<myPoint>::iterator sitr=points.begin(); sitr!=points.end(); sitr++)
  {
    int x = sitr->x(), y = sitr->y();
    if (y < min_y)
    {
      min_y = y;
      min_x = next_x = INT_MAX;
    }
    if (y > min_y) continue;

    // here: y == min_y
    if (x < min_x)
    {
      next_x = min_x;
      min_x = x;
    }
    else if (x > min_x && x < next_x)
    {
      next_x = x;
    }
  }
  Pk.set(min_x, min_y);
  Pl.set(next_x, min_y);

  // second round, determine Ym (next_y)
  int Ym = getHorRangeNext(points, Pk.x(), Pl.x(), min_y);
  //int Ym = INT_MAX;
  //for (int i=0; i<points.size(); i++)
  //{
  //  int x = points[i].x();
  //  int y = points[i].y();
  //  if (y <= min_y) continue;
  //  if (x < Pk.x() || x >= Pl.x()) continue;
  //  if (Ym > y) Ym = y;
  //}

  // third round, determine Xm
  int Xm = INT_MAX;
  for (std::set<myPoint>::iterator sitr=points.begin(); sitr!=points.end(); sitr++)
  {
    int x = sitr->x(), y = sitr->y();
    if (y != Ym) continue;
    if (Xm > x) Xm = x;
  }

  #ifdef _DEBUG_PTR
  if (Ym >= INT_MAX || Xm >= INT_MAX)
  {
    std::cout << "DEBUG| PTR::findPkPlPm(), output the points" << std::endl;
    for (std::set<myPoint>::iterator sitr=points.begin(); sitr!=points.end(); sitr++)
    {
      std::cout << sitr->x() << ", " << sitr->y() << std::endl;
    } // for sitr
  }
  #endif
  if (Ym >= INT_MAX) return false;
  if (Xm >= INT_MAX) return false;

  Pm.set(Xm, Ym);
  return true;
}
//}}}



// Similar to findPkPlPm, but try to find cut along with vertical edges
inline bool
PTR::findVkVlVm(const std::vector<myPoint>& points, myPoint& Vk, myPoint& Vl, myPoint& Vm)
//{{{
{
  if (points.size() < 4) return false;

  int min_x = INT_MAX;
  int min_y = INT_MAX, next_y = INT_MAX;

  // Step 1: 1st round, determine Vk, Vl
  for (int i=0; i<points.size(); i++)
  {
    int x = points[i].x(), y = points[i].y();
    if (x < min_x)
    {
      min_x = x;
      min_y = next_y = INT_MAX;
    }
    if (x > min_x) continue;

    // here: x == min_x
    if (y < min_y)
    {
      next_y = min_y;
      min_y = y;
    }
    else if (y > min_y && y < next_y)
    {
      next_y = y;
    }
  } // for i
  Vk.set(min_x, min_y);
  Vl.set(min_x, next_y);


  // Step 2: determine (next_x)
  int Xm = getVerRangeNext(points, Vk.y(), Vl.y(), min_x);
#if 1
  int next_x = INT_MAX;
  for (int i=0; i<points.size(); i++)
  {
    int x = points[i].x(), y = points[i].y();
    if (x <= min_x)                continue;
    if (y < Vk.y() || y > Vl.y()) continue;
    if (next_x > x) next_x = x;
  }
  assert(Xm == next_x);
#endif


  // Step 3: determine Ym
  int Ym = INT_MAX;
  for (int i=0; i<points.size(); i++)
  {
    int x = points[i].x(), y = points[i].y();
    if (x != Xm) continue;
    if (Ym > y) Ym = y;
  }

  if (Xm >= INT_MAX) return false;
  if (Ym >= INT_MAX) return false;

  Vm.set(Xm, Ym);
  return true;
}
//}}}

inline bool
PTR::findVkVlVm(const std::set<myPoint>& points, myPoint& Vk, myPoint& Vl, myPoint& Vm)
//{{{
{
  if (points.size() < 4) return false;

  int min_x = INT_MAX;
  int min_y = INT_MAX, next_y = INT_MAX;

  // Step 1: 1st round, determine Vk, Vl
  for (std::set<myPoint>::iterator sitr=points.begin(); sitr!=points.end(); sitr++)
  {
    int x = sitr->x(), y = sitr->y();
    if (x < min_x)
    {
      min_x = x;
      min_y = next_y = INT_MAX;
    }
    if (x > min_x) continue;

    // here: x == min_x
    if (y < min_y)
    {
      next_y = min_y;
      min_y = y;
    }
    else if (y > min_y && y < next_y)
    {
      next_y = y;
    }
  } // for i
  Vk.set(min_x, min_y);
  Vl.set(min_x, next_y);


  // Step 2: determine (next_x)
  int Xm = getVerRangeNext(points, Vk.y(), Vl.y(), min_x);
#if 1
  int next_x = INT_MAX;
  for (std::set<myPoint>::iterator sitr=points.begin(); sitr!=points.end(); sitr++)
  {
    int x = sitr->x(), y = sitr->y();
    if (x <= min_x)                continue;
    if (y < Vk.y() || y > Vl.y()) continue;
    if (next_x > x) next_x = x;
  }
  assert(Xm == next_x);
#endif


  // Step 3: determine Ym
  int Ym = INT_MAX;
  for (std::set<myPoint>::iterator sitr=points.begin(); sitr!=points.end(); sitr++)
  {
    int x = sitr->x(), y = sitr->y();
    if (x != Xm) continue;
    if (Ym > y) Ym = y;
  }

  if (Xm >= INT_MAX) return false;
  if (Ym >= INT_MAX) return false;

  Vm.set(Xm, Ym);
  return true;
}
//}}}


// F function
inline void
PTR::F(std::vector<myPoint>& points, int X, int Y)
//{{{
{
  myPoint point(X, Y);
  std::vector<myPoint>::iterator itr = find(points.begin(), points.end(), point);
  //points.find(point);
  
  if (itr == points.end())  // can NOT find, insert point(X, Y)
  {
    points.push_back( point );
    #ifdef _DEBUG_PTR
    printf ("append point (%d, %d) \n", point.x(), point.y());
    //print(points);
    #endif
  }
  else                      // can find, remove point (X, Y)
  {
    points.erase( itr );
    #ifdef _DEBUG_PTR
    printf ("remove point (%d, %d) \n", point.x(), point.y());
    //print(points);
    #endif
    #if 1
    itr = find(itr, points.end(), point);
    while (itr != points.end())
    {
      points.erase( itr );
      itr = find(itr, points.end(), point);
    }
    #endif
  }
}
//}}}

inline void
PTR::F(std::set<myPoint>& points, int X, int Y)
//{{{
{
  myPoint point(X, Y);
  std::set<myPoint>::iterator itr = find(points.begin(), points.end(), point);
  //points.find(point);
  
  if (itr == points.end())  // can NOT find, insert point(X, Y)
  {
    points.insert( point );
    #ifdef _DEBUG_PTR
    printf ("append point (%d, %d) \n", point.x(), point.y());
    //print(points);
    #endif
  }
  else                      // can find, remove point (X, Y)
  {
    points.erase( itr );
    #ifdef _DEBUG_PTR
    printf ("remove point (%d, %d) \n", point.x(), point.y());
    //print(points);
    #endif
    #if 0
    itr = find(itr, points.end(), point);
    while (itr != points.end())
    {
      points.erase( itr );
      itr = find(itr, points.end(), point);
    }
    #endif
  }
}
//}}}


// use kind of stupid method
// smooth the polygons, i.e., reduce the corner point number
inline void
PTR::compress(std::vector<myPoint>& points, int difference)
//{{{
{
  bool bmodify = true;
  while (true == bmodify)
  {
    bmodify = false;
    for (int i = 0; i<points.size(); i++)
    {
      int x1 = points[i].x();
      int y1 = points[i].y();
      for (int j=i+1; j<points.size(); j++)
      {
        int x2 = points[j].x();  int y2 = points[j].y();
        if ( x1 != x2 && abs(x1 - x2) <= difference )
        {
          points[i].set(std::min(x1, x2), y1);
          points[j].set(std::min(x1, x2), y2);
          bmodify = true;
        }
        if ( y1 != y2 && abs(y1 - y2) <= difference )
        {
          points[i].set(x1, std::min(y1, y2));
          points[j].set(x2, std::min(y1, y2));
          bmodify = true;
        }
      } // for j
    } // for i
  } // while (bmodify)
}
//}}}


// find a min value next_y, s.t.,
// 1) next_y > min_y
// 2) the point is in the range of [x1, x2]
inline int
PTR::getHorRangeNext(const std::vector<myPoint>& points,
                     const int x1, const int x2, const int min_y)
//{{{
{
  int next_y = INT_MAX / 2;

  for (int i=0; i<points.size(); i++)
  {
    int x = points[i].x(), y = points[i].y();
    if (x<x1 || x>x2) continue;
    if (y <= min_y)   continue;
    if (y >= next_y)  continue;  

    next_y = y;
  } // for i

  return next_y;
}
//}}}

inline int
PTR::getHorRangeNext(const std::set<myPoint>& points,
                     const int x1, const int x2, const int min_y)
//{{{
{
  int next_y = INT_MAX / 2;

  for (std::set<myPoint>::iterator sitr=points.begin(); sitr!=points.end(); sitr++)
  {
    int x = sitr->x(), y = sitr->y();
    if (x<x1 || x>x2) continue;
    if (y <= min_y)   continue;
    if (y >= next_y)  continue;  

    next_y = y;
  } // for i

  return next_y;
}
//}}}


// find a min value next_x, s.t.,
// 1) next_x > min_x
// 2) the point is in the range of [y1, y2]
inline int
PTR::getVerRangeNext(const std::vector<myPoint>& points,
                     const int y1, const int y2, const int min_x)
//{{{
{
  int next_x = INT_MAX / 2;
  
  for (int i=0; i<points.size(); i++)
  {
    int x = points[i].x(), y = points[i].y();
    if (y<y1 || y>y2) continue;
    if (x <= min_x)   continue;
    if (x >= next_x)  continue;

    next_x = x;
  } // for i

  return next_x;
}
//}}}

inline int
PTR::getVerRangeNext(const std::set<myPoint>& points,
                     const int y1, const int y2, const int min_x)
//{{{
{
  int next_x = INT_MAX / 2;
  
  for (std::set<myPoint>::iterator sitr=points.begin(); sitr!=points.end(); sitr++)
  {
    int x = sitr->x(), y = sitr->y();
    if (y<y1 || y>y2) continue;
    if (x <= min_x)   continue;
    if (x >= next_x)  continue;

    next_x = x;
  } // for i

  return next_x;
}
//}}}


// check whether three continuous points are on the same line
inline bool
PTR::findLine(const std::vector<myPoint> points)
//{{{
{
  for (int i=0; i<points.size(); i++)
  {
    int j = i+1; int k = i+2;
    if (j >= points.size()) continue;
    if (k >= points.size()) continue;
    if (points[i].x() == points[j].x() && points[i].x() == points[k].x())
    {
      #ifdef _DEBUG_PTR
        printf ("DEBUG| same x: %d %d %d\n", points[i].x(), points[j].x(), points[k].x());
      #endif
      return true;
    }
    if (points[i].y() == points[j].y() && points[i].y() == points[k].y())
    {
      #ifdef _DEBUG_PTR
        printf ("DEBUG| same y: %d %d %d\n", points[i].y(), points[j].y(), points[k].y());
      #endif
      return true;
    }
  } // for i
  return false;
}
//}}}


inline void
PTR::print(const std::vector<myPoint> points, int min_x, int min_y)
//{{{
{
  printf ("points: ");
  for (int i=0; i<points.size(); i++)
  {
    int x = points[i].x();
    int y = points[i].y();
    printf ("(%d, %d) ", x-min_x, y-min_y);
  }
  printf ("\n");
}
//}}}

inline void
PTR::print(const std::set<myPoint> points, int min_x, int min_y)
//{{{
{
  printf ("points: ");
  for (std::set<myPoint>::iterator sitr=points.begin(); sitr!=points.end(); sitr++)
  {
    int x = sitr->x(), y = sitr->y();
    printf ("(%d, %d) ", x-min_x, y-min_y);
  }
  printf ("\n");
}
//}}}

} // namespace bLib

#endif


