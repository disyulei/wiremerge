#include "myShape.h"



bool myfunction(myBox* mybox1, myBox* mybox2)
/*{{{*/
{
  oaBox* box1 = mybox1->m_interbox;
  oaBox* box2 = mybox2->m_interbox;

  if (box1->left()==box2->left() && box1->right()==box2->right())
    {
      return (box1->bottom() < box2->bottom());
    }
  
  if (box1->bottom()==box2->bottom() && box1->top()==box2->top())
    {
      return (box1->left() < box2->left());
    }
  //PrintBBox(box1);
  //PrintBBox(box2);
  return 0;
  //throw SEGMENT_SHAPES_NOT_ALIGHED;
}
/*}}}*/


// *************************************************************
//                  Functions for myShape
// *************************************************************
myShape::myShape(int xl, int yl, int xh, int yh)
{
  // assign all the ids
  m_id   = -1;

  m_realBox = new oaBox(xl, yl, xh, yh);
  m_realBoxes.clear();
}

myShape::myShape()
{
  m_id = -1;
  m_realBox = new oaBox();
}

myShape::~myShape()
{
  delete m_realBox;

  for (int i=0; i<m_realBoxes.size(); i++) {
    delete m_realBoxes[i]->m_interbox;
    delete m_realBoxes[i];
  } // for i
}

void
myShape::SetPoints( oaPointArray & vpoints )
{
  int size = vpoints.getSize();
  for (int i=0; i<size; i++) {
    oaPoint point = vpoints[i];
    m_vpoints.append( point );
  } // for i
}

void
myShape::SetBox(int xl, int yl, int xh, int yh)
{
  m_realBox->set(xl, yl, xh, yh);
  assert(m_realBox->left() == xl);
  assert(m_realBox->right() == xh);
  assert(m_realBox->bottom() == yl);
}

void
myShape::SetRealBoxes(vector<oaBox> & vBoxes)
{
  for (int i=0; i<vBoxes.size(); i++) {
    oaBox* box   = new oaBox( vBoxes[i] );
    myBox* mybox = new myBox( box );
    m_realBoxes.push_back(mybox);
  } // for i
}

void
myShape::print()
{
  cout << "(" <<m_realBox->left()<< "," << m_realBox->bottom() << ")" <<"-" <<"("<<m_realBox->right() << "," << m_realBox->top() << ")\t\t";
}

int
myShape::GetAreas()
{
  return m_realBox->getWidth() * m_realBox->getHeight();
#if 0
  int result = 0;
  int size = m_realBoxes.size();
  for (int i=0; i<size; i++) {
    myBox* pmybox = m_realBoxes[i];
    result += pmybox->getArea();
  }
  return result;
#endif
}



