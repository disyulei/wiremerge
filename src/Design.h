#ifndef _DESIGN_H_
#define _DESIGN_H_

#include "bLib/bLibPtr.h"
#include "bLib/bLibShape.h"
#include "bLib/bLibRTree.h"
#include "main.h"

class Design
{
public:
  Design();
  ~Design();

  bool parseParameters(int argc, char** argv);
  void readAll();
  void mergeWires();
  void OutputASCII_new();     // output to new ascii format used in DAC'12
  void OutputASCII();         // output my current ascii format 

protected:
  std::string library_;
  int         layerMerge_;              // layer to be merged
  int         layerMax_;
  double      ratio_;

  std::vector< std::vector<bLib::myShape*> > m_Metals;
  bLib::bLibRTree<bLib::myShape>             m_rtree;

  std::vector< std::vector<int> >   m_MergedWireIDs;
  std::vector<Polygon>              m_Polygons;

  // functions
  void   ReadASCII();
  void   TouchComponentCompute_RTree();
  bool   ReadSearchUntil( std::ifstream& in, std::string str1, std::string str2="" );
  bool   ReadBlock( std::ifstream& in );
  int    ReadLayerNum  ( std::ifstream&, std::string );
  double ReadUnitsValue( std::ifstream&, std::string );

  // output functions
  void   Debug_WireInput(std::string file="debug/wire_merge_input.tcl");
  void   Debug_Output(std::string file="debug/wire_merge_output.tcl");
};

#endif
