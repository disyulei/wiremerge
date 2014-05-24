#include "WireRTree.h"

vector<myShape*> WireRTree::s_searchResult;

void
WireRTree::insert(myShape* myshape)
{
  oaBox* bbox = myshape->m_realBox;
  //RtreeRect rect(bbox->left(), bbox->bottom(), bbox->right(), bbox->top());
  RtreeRect rect(bbox->left()-1, bbox->bottom()-1, bbox->right()+1, bbox->top()+1); /////
  m_tree.Insert(rect.ll, rect.ur, myshape);
}

vector<myShape*>*
WireRTree::searchoaBox(oaBox* bbox)
{
  RtreeRect rect(bbox->left(), bbox->bottom(), bbox->right(), bbox->top());
  s_searchResult.clear();
  m_tree.Search(rect.ll, rect.ur, rtreeSearchCB, NULL);
  return &s_searchResult;
}

bool
WireRTree::rtreeSearchCB(myShape* data, void* arg)
{
  s_searchResult.push_back(data);
  return true;    //keep going
}

