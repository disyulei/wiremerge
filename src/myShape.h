#ifndef _MYSHAPE_H_
#define _MYSHAPE_H_
#include "main.h"

// ******************************************************
// 
//   myBox:        rectangle box
//   myShape:      polygon
//
// ******************************************************

class myBox;

class myShape
{
 public:
  typedef int (myShape::*FP) ();
  myShape();
  myShape(int, int, int, int);
  ~myShape();

  int     GetAreas();
  oaBox*  GetoaBox() { return m_realBox; }
  int     GetID()    { return m_id; }
  int     left()     { return m_realBox->left(); }
  int     right()    { return m_realBox->right(); }
  int     bottom()   { return m_realBox->bottom();}
  int     top()      { return m_realBox->top();}

  void    SetID(int id) { m_id = id; }
  void    SetBox(int xl, int yl, int xh, int yh);
  void    SetRealBoxes(vector<oaBox> & vboxes);
  void    SetPoints( oaPointArray & vpoints );

	void    print();     // debug for Bei

  int             m_id;	        // wire id for current layer
  oaPointArray    m_vpoints;
  oaBox*          m_realBox;    // for "polygon" store its bounding box
  vector<myBox* > m_realBoxes;  // rectangles after input
                                // for "polygon", store its decomposed rectangles
};


class myBox
//{{{
{
 public:
  myBox(oaBox* pBox) { m_id=-1; m_interbox = pBox; }

  oaBox*       m_interbox;
  myShape*     m_myShape;
  set<myBox* > m_TouchBoxes; // stitch candidates
  set<myBox* > m_DPLBoxes;   // conflict candidates

  // *********************************************************
  //          Geometry Operation Functions
  // *********************************************************
  int  x1() const;  int  x2() const;
  int  y1() const;  int  y2() const;
  int  dx() const;
  int  dy() const;
  bool isHor() const;
  int  getArea();

  int left()   {return x1();}
  int right()  {return x2();}
  int bottom() {return y1();}
  int top()    {return y2();}

  // **********************************************************
  //               All the IDs
  // **********************************************************
  int  m_id;			    // temperal id
};
//}}}



// *******************************************************
//             Inline functions for myBox
// *******************************************************
//{{{
inline
int
myBox::getArea()
{
  return m_interbox->getWidth() * m_interbox->getHeight();
}

inline
int
myBox::x1() const
{
  return m_interbox->left();
}

inline
int
myBox::x2() const
{
  return m_interbox->right();
}

inline
int
myBox::y1() const
{
  return m_interbox->bottom();
}

inline
int
myBox::y2() const
{
  return m_interbox->top();
}

inline
int
myBox::dx() const
{
  return x2() - x1();
}

inline
int
myBox::dy() const
{
  return y2() - y1();
}

inline
bool
myBox::isHor() const
{
  return (dx() >= dy());
}
//}}}

#endif
