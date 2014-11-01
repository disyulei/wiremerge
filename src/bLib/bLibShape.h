#ifndef _B_LIB_SHAPE_H_
#define _B_LIB_SHAPE_H_

// ============================================================== //
//   Library for geometrical shapes 
//
//   Class       : bBox, bShape
//   Author      : Bei Yu
//   Last update : 05/2014
// ============================================================== //

#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include "bLibPoint.h"

namespace bLib
{

// ==========================================
//            bBox
// ==========================================
class bBox
{
public:
  bBox(int x1=-1, int y1=-1, int x2=-1, int y2=-1); 

  // get functions
  int   getId()    const {return m_id;}
  int   x1()       const {return m_x1;}
  int   x2()       const {return m_x2;}
  int   y1()       const {return m_y1;}
  int   y2()       const {return m_y2;}
  int   dx()       const {return x2() - x1();}
  int   dy()       const {return y2() - y1();}
  int   getArea()  const {return dx()*dy();}
  int   getWidth() const {return std::min(dx(), dy());}
  int   xCenter()        {return (x1()+x2())/2;}
  int   yCenter()        {return (y1()+y2())/2;}
  int   getDist2Box(const bBox*, int=12345678) const;
  int   getDrct2Box(bBox*);
  void  intersection(bBox&,int&,int&,int&,int&);
  bool  isHor()          {return (dx() >= dy());}
  bool  overlaps(bBox*, bool=true);
  bool  isContain(const int, const int) const;

  // set functions
  void  set(int, int, int, int);
  void  setId(int id) {m_id = id;}

protected:
  int   m_x1,  m_x2, m_y1,  m_y2;
  int   m_id;
};
inline
std::ostream& operator << (std::ostream& out, bBox& mybox)
{
  out<<mybox.getId()<<"| ("<<mybox.x1()<<","<<mybox.y1()<<")  ("<<mybox.x2()<<","<<mybox.y2()<<")";
  return out;
}

// =============================================
//        functions for bBox
// =============================================
//{{{
inline
bBox::bBox(int x1, int y1, int x2, int y2):m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2)
{
  if (m_x1 > m_x2) std::swap(m_x1, m_x2);
  if (m_y1 > m_y2) std::swap(m_y1, m_y2);
  m_id = -1;
}

inline bool
bBox::isContain(const int x, const int y) const
{
  if (m_x1!=x && m_x2!=x) return false;
  if (m_y1!=y && m_y2!=y) return false;
  return true;
}
inline void
bBox::set(int x1, int y1, int x2, int y2)
{
  m_x1 = x1;
  m_y1 = y1;
  m_x2 = x2;
  m_y2 = y2;
  if (m_x1 > m_x2) std::swap(m_x1, m_x2);
  if (m_y1 > m_y2) std::swap(m_y1, m_y2);
}

inline bool
bBox::overlaps(bBox* box, bool flag)
//{{{
{
  int width1  = dx();
  int height1 = dy();
  if (width1 <=0 || height1 <= 0) return false;
  int width2  = box->dx();
  int height2 = box->dy();
  if (width2 <=0 || height2 <= 0) return false;

  // following are from oaBox.cpp
  if (flag)
  {
    return !(x1() > box->x2() || y1() > box->y2()
          || x2() < box->x1() || y2() < box->y1());
  }

    return !(x1() >= box->x2() || y1() >= box->y2()
          || x2() <= box->x1() || y2() <= box->y1());
}
//}}}

inline void
bBox::intersection(bBox& pbox, int& xx1, int& yy1, int& xx2, int& yy2)
{
  xx1 = std::max(x1(), pbox.x1());
  yy1 = std::max(y1(), pbox.y1());
  xx2 = std::min(x2(), pbox.x2());
  yy2 = std::min(y2(), pbox.y2());
}

// 7 | 3 | 6
// __|___|__
// 1 | 8 | 2
// __|___|__
// 4 | 0 | 5
inline int
bBox::getDrct2Box(bBox* box2)
//{{{
{
  int oct;
  int cx = (int)this->x1();
	int cy = (int)this->y1(); 
  int cw = (int)this->dx();
	int ch = (int)this->dy();
  int ox = (int)box2->x1(); 
	int oy = (int)box2->y1(); 
  int ow = (int)box2->dx(); 
	int oh = (int)box2->dy();

  if(cx + cw <= ox)    
    if(cy + ch <= oy)		   oct = 4;		
    else if(cy >= oy + oh) oct = 7;
    else                   oct = 1;
  else if(cx >= ox + ow)
    if(cy + ch <= oy)      oct = 5;
    else if(cy >= oy + oh) oct = 6;
    else                   oct = 2;
  else if(cy + ch <= oy)   oct = 0;
  else if(cy >= oy + oh)   oct = 3;
  else                     oct = 8;
  return oct;
}
//}}}

inline int
bBox::getDist2Box(const bBox* box2, int MAX_DIST) const
//{{{
{
  int oct;
	// 6 | 5 | 4
	// __|___|__
	// 7 | 9 | 3
	// __|___|__
	// 0 | 1 | 2
  int cx, cy, cw, ch, ox, oy, ow, oh;
  cx = (int)this->x1();
	cy = (int)this->y1();
  cw = (int)this->dx(); 
	ch = (int)this->dy();
  ox = (int)box2->x1(); 
	oy = (int)box2->y1();
  ow = (int)box2->dx(); 
	oh = (int)box2->dy();

  if(cx + cw <= ox)    
    if(cy + ch <= oy)		   oct = 0;		
    else if(cy >= oy + oh) oct = 6;
    else                   oct = 7;
  else if(cx >= ox + ow)
    if(cy + ch <= oy)      oct = 2;
    else if(cy >= oy + oh) oct = 4;
    else                   oct = 3;
  else if(cy + ch <= oy)   oct = 1;
  else if(cy >= oy + oh)   oct = 5;
  else  return 0;

  int result = 0;
  int temp = 0;
  
  switch(oct) {
    case 0:
      cx = (cx + cw) - ox;
      cy = (cy + ch) - oy;
      if (abs(cx) > MAX_DIST) return MAX_DIST;
      if (abs(cy) > MAX_DIST) return MAX_DIST;
      result = (int)std::sqrt(cx*cx + cy*cy); 
      break;
    case 1:
      temp = abs((cy+ch) - oy);
      if (temp > MAX_DIST)    return MAX_DIST;
      result = temp;
      break;
    case 2:
      cx = (ox + ow) - cx;
      cy = (cy + ch) - oy;
      if (abs(cx) > MAX_DIST) return MAX_DIST;
      if (abs(cy) > MAX_DIST) return MAX_DIST;
      result = (int)std::sqrt(cx*cx + cy*cy);
      break;
    case 3:
      temp = abs((ox + ow) -cx);
      if (temp > MAX_DIST)    return MAX_DIST;
      result = temp;
      break;
    case 4:
      cx = (ox + ow) - cx;
      cy = (oy + oh) - cy;
      if (abs(cx) > MAX_DIST) return MAX_DIST;
      if (abs(cy) > MAX_DIST) return MAX_DIST;
      result = (int)std::sqrt(cx*cx + cy*cy);
      break;
    case 5:
      temp = abs((oy+oh) - cy);
      if (temp > MAX_DIST)    return MAX_DIST;
      result = temp;
      break;
    case 6:
      cx = (cx + cw) - ox;
      cy = (oy + oh) - cy;
      if (abs(cx) > MAX_DIST) return MAX_DIST;
      if (abs(cy) > MAX_DIST) return MAX_DIST;
      result = (int)std::sqrt(cx*cx + cy*cy);
      break;
    case 7:
      temp = abs ( (cx+cw) - ox );
      if (temp > MAX_DIST)    return MAX_DIST;
      result = temp;
      break;
    default:
      break;
    }

#ifdef _DEBUG_BEI
  if (result < 0)
    std::cout<<std::endl<< "temp = " << temp << "  oct = " << oct << "  cx = " << cx << "  cy = " << cy << std::endl;
#endif
  return (result > MAX_DIST)? MAX_DIST : result;
}
//}}}


//}}}



// ==========================================
//              bShape
// ==========================================
class bShape
{
public:
  bShape(int,int,int,int);
  ~bShape();

  // ==== get functions
  int    getId()   const  {return m_id;}
  int    x1()      const  {return m_realBox->x1();}
  int    x2()      const  {return m_realBox->x2();}
  int    y1()      const  {return m_realBox->y1();}
  int    y2()      const  {return m_realBox->y2();}
  int    dx()      const  {return x2() - x1();}
  int    dy()      const  {return y2() - y1();}
  int    xCenter() const  {return (x1()+x2())/2;}
  int    yCenter() const  {return (y1()+y2())/2;}
  int    width()   const  {return x2() - x1();}
  int    height()  const  {return y2() - y1();}
  bool   isHor()   const  {return width() >= height();}
  int    getPointNum()    {return m_vpoints.size();}
  int    getPointX(int);
  int    getPointY(int);
  int    getArea();
  std::vector<bPoint> const& getVPoints() const {return m_vpoints;}

  // ==== set functions
  void   setId(int id)    {m_id = id;}
  void   setPoints(const std::vector<bPoint>&);
  void   setBox(int, int, int, int);
  void   setRealBoxes(const std::vector<bBox>&);

  std::vector<bBox*>   m_realBoxes;  // rectangles after input

protected:
  void   points2Boxes();              // given m_vpoints, generate m_realBoxes

  int                   m_id;
  bBox*                 m_realBox;    // for "polygon" store its bounding box
  std::vector<bPoint>   m_vpoints;    // clock-wise points to represent polygon
};
inline
std::ostream& operator << (std::ostream& out, bShape& myshape)
{
  out<<myshape.getId()<<": ("<<myshape.x1()<<","<<myshape.y1()<<") ("<<myshape.x2()<<","<<myshape.y2()<<") ";
  return out;
}

// =============================================
//        functions for bShape
// =============================================
//{{{
inline
bShape::bShape(int xl, int yl, int xh, int yh)
{
  m_id = -1;
  m_realBox = new bBox(xl, yl, xh, yh);
  m_realBoxes.clear();
}

inline
bShape::~bShape()
{
  delete m_realBox;
  for (int i=0; i<m_realBoxes.size(); i++) delete m_realBoxes[i]; m_realBoxes.clear();
}

inline int
bShape::getPointX(int id)
{
  assert (id < m_vpoints.size());
  return m_vpoints[id].x();
}

inline int
bShape::getPointY(int id)
{
  assert (id < m_vpoints.size());
  return m_vpoints[id].y();
}

inline void
bShape::setPoints(const std::vector<bPoint>& vpoints)
{
  int size = vpoints.size();
  for (int i=0; i<size; i++)
  {
    bPoint point(vpoints[i].x(), vpoints[i].y());
    m_vpoints.push_back( point );
  } // for i
}

inline void
bShape::setBox(int xl, int yl, int xh, int yh)
{
  m_realBox->set(xl, yl, xh, yh);
  assert(m_realBox->x1() == xl);
  assert(m_realBox->x2() == xh);
  assert(m_realBox->y1() == yl);
}

// given m_vpoints, generate m_realBoxes
inline void
bShape::setRealBoxes(const std::vector<bBox>& vBoxes)
{
  for (int i=0; i<vBoxes.size(); i++)
  {
    bBox* pmybox = new bBox( vBoxes[i] );
    m_realBoxes.push_back(pmybox);
  }
}

inline int
bShape::getArea()
{
  int result = 0;
  for (int i=0; i<m_realBoxes.size(); i++) result += m_realBoxes[i]->getArea();
  return result;
}

//}}}


// =============================================
//       global geometrical functions
// =============================================
inline int
box2BoxDirection(bBox* box1, bBox* box2)
{
  return box1->getDrct2Box(box2);
}

inline int
box2BoxDist(const bBox* box1, const bBox* box2, int MAX_DIST=12345678)
{
  return box1->getDist2Box(box2, MAX_DIST);
}

inline int
box2BoxDistX(bBox* box1, bBox* box2)
{
  int direction = box2BoxDirection(box1, box2);
  switch (direction)
  {
    case 1: return 0;
    case 7: return box1->y1() - box2->y2();
    case 4: return box2->y1() - box1->y2();
    default: std::cout<<"WARNING| myBox2BoxDistY(), direction = "<<direction<<std::endl;
  }
  return 0;
}

inline int
box2BoxDistY(bBox* box1, bBox* box2)
{
  assert( box1->x2() <= box2->x1() );
  return box2->x1() - box1->x2();
}

} // namespace bLib

/*
// ==== Implementation Log:
//
// 07/2014: rename myBox==>bBox, myShape==>bShape
// 
// 05/2014: introduce namespace bLib
//
*/

#endif

