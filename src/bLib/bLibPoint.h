#ifndef _B_LIB_POINT_H_
#define _B_LIB_POINT_H_

// ============================================================== //
//   Library for simple geometrical shapes 
//
//   Class       : myPoint, mySegment 
//   Author      : Bei Yu
//   Last update : 06/2014
// ============================================================== //

#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

namespace bLib
{

// ==========================================
//             myPoint
// ==========================================
struct myPoint
{
  myPoint(const myPoint& p):   m_x(p.x()), m_y(p.y()) {}
  myPoint(int x=-1, int y=-1): m_x(x),     m_y(y) {}

  int  x() const         {return m_x;}
  int  y() const         {return m_y;}
  void set(int x, int y) {m_x = x; m_y = y;}

  int   m_x;
  int   m_y;
};
inline
bool operator == (const myPoint& p1, const myPoint& p2)
{
  return (p1.x()==p2.x() && p1.y()==p2.y());
}
inline
bool operator < (const myPoint& p1, const myPoint& p2)
{
  if (p1.x() < p2.x()) return true;
  else if (p1.x() > p2.x()) return false;
  else return (p1.y() < p2.y());
}


// ==========================================
//             mySegment
// ==========================================
struct mySegment
{
};

} // namespace bLib

#endif

