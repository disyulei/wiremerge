#ifndef _MAIN_H_
#define _MAIN_H_

#include <cassert>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <queue>
#include <stack>
#include <iomanip>
#include <algorithm>


// ================================================
//                  for boost
// ================================================
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/polygon/polygon.hpp>
namespace gtl = boost::polygon;
using namespace boost::polygon::operators;


//#define _DEBUG_BEI
//#define _DEBUG_PTR

typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS> Graph;
typedef gtl::polygon_data<int> Polygon;
typedef gtl::rectangle_data<int> Rectangle;
//typedef gtl::polygon_traits<Polygon>::point_type Point;
typedef gtl::point_data<int> Point;

#endif
