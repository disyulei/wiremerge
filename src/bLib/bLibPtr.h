#ifndef _B_LIB_PTR_H_
#define _B_LIB_PTR_H_

// ===================================================================
//    class       : PTR
//    author      : Bei Yu
//    last update : 11/2014
//
//  Refer to K.D.Gourley and D.M. Green,
//  "A Polygon-to-Rectangle Conversion Algorithm", IEEE 1983
//
// ===================================================================

#include "bLibBase.h"
#include "bLibPoint.h"
#include <iostream>
#include <algorithm>  // min()
#include <vector>
#include <climits>
#include <cstdio>

namespace bLib
{

//#define _DEBUG_PTR
class PTR
{
public:
  template<class Type>
  static bool polygon2Rect(std::vector<bPoint>& points, std::vector<Type> & vBoxes, int flag=-1);

private:
  static void removeDuplicate(std::vector<bPoint>&);
  static bool findLine(const std::vector<bPoint> points);
  static bool findPkPlPm(const std::vector<bPoint>&, bPoint&, bPoint&, bPoint&);
  static bool findVkVlVm(const std::vector<bPoint>&, bPoint&, bPoint&, bPoint&);
  static void F(std::vector<bPoint>& points, int X, int Y);
  static int  getHorRangeNext(const std::vector<bPoint>&,const int,const int,const int);
  static int  getVerRangeNext(const std::vector<bPoint>&,const int,const int,const int);
  static void print(const std::vector<bPoint> points, int min_x = 0, int min_y = 0);
};

// ============================================
//              inline functions
// ============================================
// Refer to [Courley+, IEEE'83]: "A Polygon-to-Rectangle Conversion Algorithm"
// NEW improvement!!
// meanwhile, reduce the slice rectangle
// Input  : points
// Output : boxes
// flag = 0 (only horiozntal cut); 1 (only vertical cut)
template<class Type>
inline bool
PTR::polygon2Rect(std::vector<bPoint>& vpoints, std::vector<Type>& boxes, int flag)
//{{{
{
#ifdef _DEBUG_PTR
  std::cout << "debug| PTR::Polygon2Rectangle(), vpoints = " << std::endl;
  print(vpoints);
#endif

  // Step 1: check polygon
  int point_num = vpoints.size();
  if (vpoints[0]==vpoints[point_num-1]) vpoints.resize( point_num-1 );
  for (int i=1; i<vpoints.size(); i++)
  {
    int x1 = vpoints[i-1].x(), x2 = vpoints[i].x();
    int y1 = vpoints[i-1].y(), y2 = vpoints[i].y();
    if (x1==x2 || y1==y2) continue;
    std::cout << "ERROR| PTR::polygon2Rect(), can only support rectlinear polygon" << std::endl;
    print(vpoints);
    return false;
  }
  if (true == findLine(vpoints))
  {
    std::cout << "ERROR| PTR::polygon2Rect(), find three continuous points are on a line" << std::endl;
    print(vpoints);
    return false;
  }
  removeDuplicate(vpoints);  // after this function, the order of points canNOT be maintained
  

  // Step 2: iteratively remove box
  while (vpoints.size() > 0)
  {
    bPoint Pk, Pl, Pm;
    bPoint Vk, Vl, Vm; // similar to Pk,Pl,Pm, but scan vertical edges now

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

    Type box;
    if (0 == flag)
    {
        // only ONE candidates: horizontal cut
        //getHorRangeNext(vpoints, Pk.x(), Pl.x(), Pm.y()) - Pm.y();
        box.set(Pk.x(), Pk.y(), Pl.x(), Pm.y());
    }
    else if (1 == flag)
    {
        // only ONE candidates: vertical cut
        //getVerRangeNext(vpoints, Vk.y(), Vl.y(), Vm.x()) - Vm.x();
        box.set(Vk.x(), Vk.y(), Vm.x(), Vl.y());
    }
    else // -1 == flag
    {
        // two candidates:
        int slide_y = getHorRangeNext(vpoints, Pk.x(), Pl.x(), Pm.y()) - Pm.y();
        int slide_x = getVerRangeNext(vpoints, Vk.y(), Vl.y(), Vm.x()) - Vm.x();
#ifdef _DEBUG_PTR
        if (slide_y >= slide_x) printf("DEBUG| select Pk, Pl. Pm\n");
        else                    printf("DEBUG| select Vk, Vl. Vm\n");
#endif
        if (slide_y >= slide_x) box.set(Pk.x(), Pk.y(), Pl.x(), Pm.y());
        else                    box.set(Vk.x(), Vk.y(), Vm.x(), Vl.y());
    }


    if (box.x2()>INT_MAX-1000 || box.y2()>INT_MAX-1000) return false;
    boxes.push_back(box);
#ifdef _DEBUG_PTR
    printf ("DEBUG| add box (%d %d), (%d %d) into output vector<myBox>\n", box.x1(), box.y1(), box.x2(), box.y2());
#endif
    
    F(vpoints, box.x1(), box.y1());
    F(vpoints, box.x1(), box.y2());
    F(vpoints, box.x2(), box.y1());
    F(vpoints, box.x2(), box.y2());

    #ifdef _DEBUG_PTR
    printf ("DEBUG| current %d points: \n", (int)vpoints.size());
    print(vpoints);
    printf ("\n");
    #endif
  } // while

  return true;
}
//}}}


inline void
PTR::removeDuplicate(std::vector<bPoint>& vpoints)
{
    std::sort(vpoints.begin(), vpoints.end(), byXY);
    for (int i=1; i<vpoints.size(); i++)
    {
        if (vpoints[i].x() != vpoints[i-1].x()) continue;
        if (vpoints[i].y() != vpoints[i-1].y()) continue;
        erase_fast(vpoints, i-1);
        erase_fast(vpoints, i);
    }
}

//  Given points, find Pk, Pl and Pm
//  Pk: the leftmost of the lowest points
//  Pl: the next leftmost of the lowest points
//  Pm: 1) Xk <= Xm < Xl
//      2) Ym is lowest but Ym > Yk (Yk == Yl)
inline bool
PTR::findPkPlPm(const std::vector<bPoint>& points, bPoint& Pk, bPoint& Pl, bPoint& Pm)
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


// Similar to findPkPlPm, but try to find cut along with vertical edges
inline bool
PTR::findVkVlVm(const std::vector<bPoint>& points, bPoint& Vk, bPoint& Vl, bPoint& Vm)
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


// F function
inline void
PTR::F(std::vector<bPoint>& points, int X, int Y)
//{{{
{
  #ifdef _DEBUG_PTR
  printf("DEBUG| PTR::F(), x=%d, y=%d\n", X, Y);
  #endif

  bPoint point(X, Y);
  std::vector<bPoint>::iterator itr = find(points.begin(), points.end(), point);
  //points.find(point);
  
  if (itr == points.end())  // can NOT find, insert point(X, Y)
  {
    points.push_back( point );
    #ifdef _DEBUG_PTR
    printf ("DEBUG| append point (%d, %d)\n", point.x(), point.y());
    //print(points);
    #endif
  }
  else                      // can find, remove point (X, Y)
  {
    erase_fast(points, itr);
    #ifdef _DEBUG_PTR
    printf ("DEBUG| remove point (%d, %d)\n", point.x(), point.y());
    //print(points);
    #endif
  }
}
//}}}


// find a min value next_y, s.t.,
// 1) next_y > min_y
// 2) the point is in the range of [x1, x2]
inline int
PTR::getHorRangeNext(const std::vector<bPoint>& points,
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


// find a min value next_x, s.t.,
// 1) next_x > min_x
// 2) the point is in the range of [y1, y2]
inline int
PTR::getVerRangeNext(const std::vector<bPoint>& points,
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


// check whether three continuous points are on the same line
inline bool
PTR::findLine(const std::vector<bPoint> points)
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
PTR::print(const std::vector<bPoint> points, int min_x, int min_y)
//{{{
{
  printf ("\tpoints: ");
  for (int i=0; i<points.size(); i++)
  {
    int x = points[i].x();
    int y = points[i].y();
    printf ("%d(%d, %d) ", i, x-min_x, y-min_y);
  }
  printf ("\n");
}
//}}}
} // namespace bLib


/*
// ==== Implementation Logs:
//
// 11/2014: fix bug for ICCAD'14 contest (again). Remove the implementation based on std::set
// 10/2014: fix bug for ICCAD'14 contest
// 07/2014: change myPoint==>bPoint
// 
*/

#endif


