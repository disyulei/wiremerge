#ifndef _B_LIB_POINT_H_
#define _B_LIB_POINT_H_

// ============================================================== //
//   Library for simple geometrical shapes 
//
//   Class       : bPoint, bSegment 
//   Author      : Bei Yu
//   Last update : 07/2014
// ============================================================== //

#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

namespace bLib
{

// ==========================================
//             bPoint
// ==========================================
struct bPoint
{
    bPoint(const bPoint& p):   m_x(p.x()), m_y(p.y()) {}
    bPoint(int x=-1, int y=-1): m_x(x),     m_y(y) {}

    int  x() const         {return m_x;}
    int  y() const         {return m_y;}
    int  getDist(const bPoint p2);

    void set(int x, int y) {m_x = x; m_y = y;}

    int   m_x;
    int   m_y;
};
inline
bool operator == (const bPoint& p1, const bPoint& p2)
{
    return (p1.x()==p2.x() && p1.y()==p2.y());
}
inline
bool operator < (const bPoint& p1, const bPoint& p2)
{
    if          (p1.x() < p2.x()) return true;
    else if     (p1.x() > p2.x()) return false;
    else return (p1.y() < p2.y());
}
inline
bool byXY(const bPoint& p1, const bPoint& p2)
{
    return p1 < p2;
}
inline
int bPoint::getDist(const bPoint p2)
{
    int cx = p2.x() - x();
    int cy = p2.y() - y();
    return sqrt(cx*cx + cy*cy);
}

inline int
point2PointDist(const int x1, const int y1, const int x2, const int y2)
{
    int cx = x2 - x1;
    int cy = y2 - y1;
    return sqrt(cx*cx + cy*cy);
}

// ==========================================
//             bSegment
// ==========================================
struct bSegment
{
};

} // namespace bLib

/*
// ==== Implementation Logs:
//
// 07/2014: rename myPoint==>bPoint, mySegment==>bSegment
// 
*/


#endif

