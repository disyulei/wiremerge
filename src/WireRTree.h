#ifndef _WIRE_RTREE_H_
#define _WIRE_RTREE_H_

#include "main.h"
#include "RTree.h"
#include "myShape.h"


struct RtreeRect
{
  RtreeRect(int llx, int lly, int urx, int ury)
  {
    ll[0] = llx;
    ll[1] = lly;
    ur[0] = urx;
    ur[1] = ury;
  }

  int ll[2];    //0:x, 1:y
  int ur[2];
};


class WireRTree
{
public:
  WireRTree() {}
  
  void insert(myShape* myshape);
  vector<myShape*>* searchoaBox(oaBox* bbox);

  static bool rtreeSearchCB(myShape* date, void* arg);
  static vector<myShape*> s_searchResult;

  // some i/o functions
  int size(){return m_tree.Count();}

private:
  RTree<myShape*, int, 2, float> m_tree;
};

#endif
