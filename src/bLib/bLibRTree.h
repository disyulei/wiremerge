#ifndef _B_LIB_RTREE_H_
#define _B_LIB_RTREE_H_

// ===================================================================
//    class       : RTree
//    author      : Bei Yu
//    last update : 05/2014
// ===================================================================

#include "bLibShape.h"
#include "RTree.h"
#include <vector>

namespace bLib
{

// ==================================
//            RtreeRect
// ==================================
struct RtreeRect
{
  RtreeRect(int llx, int lly, int urx, int ury)
  {
    ll[0] = llx;   ll[1] = lly;
    ur[0] = urx;   ur[1] = ury;
  }
  int ll[2], ur[2];
};

// ==================================
//           bLibRTree
// ==================================
template<class Type>
class bLibRTree
{
public:
  bLibRTree() {}

  void   insert(Type*);
  int    getSize();  // count the data in the tree, but slow
  std::vector<Type*>* search(myBox* bbox);
  std::vector<Type*>* search(const int, const int, const int, const int);

  static std::vector<Type*> s_searchResult;

protected:
  static bool rtreeSearchCB(Type* date, void* arg);

  RTree<Type*, int, 2, float> m_tree;
};

// ==== inline functions for bLibRTree
// count the data in the tree, but slow
template<class Type>
inline int bLibRTree<Type>::getSize()
{
  return m_tree.Count();
}

template<class Type>
inline void
bLibRTree<Type>::insert(Type* pshape)
{
  RtreeRect rect(pshape->x1(), pshape->y1(), pshape->x2(), pshape->y2());
  m_tree.Insert(rect.ll, rect.ur, pshape);
}

template<class Type>
inline std::vector<Type*>*
bLibRTree<Type>::search(myBox* bbox)
{
  RtreeRect rect(bbox->x1(), bbox->y1(), bbox->x2(), bbox->y2());
  s_searchResult.clear();
  m_tree.Search(rect.ll, rect.ur, rtreeSearchCB, NULL);
  return &s_searchResult;
}

template<class Type>
inline std::vector<Type*>*
bLibRTree<Type>::search(const int x1, const int y1, const int x2, const int y2)
{
  RtreeRect rect(x1, y1, x2, y2);
  s_searchResult.clear();
  m_tree.Search(rect.ll, rect.ur, rtreeSearchCB, NULL);
  return &s_searchResult;
}

template<class Type>
inline bool
bLibRTree<Type>::rtreeSearchCB(Type* data, void* arg)
{
  s_searchResult.push_back(data);
  return true;    //keep going
}

template<class Type>
std::vector<Type*> bLibRTree<Type>::s_searchResult;

} // namespace bLib

/*
// ==== Implementation Log:
//
// 05/2014: new function search(const int, const int, const int, const int)
//
*/

#endif

