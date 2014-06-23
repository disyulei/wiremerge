#ifndef _DESIGN_H_
#define _DESIGN_H_

// ================================================
//                  for boost
// ================================================
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/polygon/polygon.hpp>
namespace gtl = boost::polygon;
using namespace boost::polygon::operators;

typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS> Graph;
typedef gtl::polygon_data<int> Polygon;
typedef gtl::rectangle_data<int> Rectangle;
typedef gtl::point_data<int> Point;

#include "bLib/bLibBase.h"
#include "bLib/bLibPtr.h"
#include "bLib/bLibShape.h"
#include "bLib/bLibRTree.h"
#include "main.h"

class Design
{
public:
  Design();
  ~Design(){}

  bool   parseParameters(int argc, char** argv);
  void   readAll();
  void   mergeWires();
  void   mergeWires(int);
  void   outputAscii();
  void   outputAscii(int, std::ofstream&);

protected:
  std::string  m_input;    // input file name
  std::string  m_output;   // output file name
  double       m_ratio;

  std::vector< std::vector<bLib::myShape*> >     m_Metals;    // input myShape
  std::vector< std::vector< std::vector<int> > > m_mergeIds;  // group some myShape together
  std::map<int, int>                             m_layer2Id;

  // local functions
  void   readAscii(int);
  double readUnitsValue(std::ifstream&, const std::string&);
  int    readLayerNum(std::ifstream&, const std::string&);
  double readBlock(int, std::ifstream&);
};

#endif


