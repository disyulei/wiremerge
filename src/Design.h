#ifndef _DESIGN_H_
#define _DESIGN_H_

#include "PTR.h"
#include "myShape.h"
#include "WireRTree.h"
#include "main.h"

class Design
{
public:
  Design();
  ~Design();

  bool parseParameters(int argc, char** argv);
  void ReadAll();
  void MergeWires();
  void OutputASCII_new();     // output to new ascii format used in DAC'12
  void OutputASCII();         // output my current ascii format 

protected:
  string      library_;
  int         layerMerge_;              // layer to be merged
  int         layerMax_;
  double      ratio_;

  vector< vector<myShape*> > m_Metals;
  WireRTree                  m_rtree;

  vector< vector<int> >      m_MergedWireIDs;
  vector<Polygon>            m_Polygons;

  // functions
  void   ReadASCII();
  void   TouchComponentCompute_RTree();
  bool   ReadSearchUntil( ifstream& in, string str1, string str2="" );
  bool   ReadBlock( ifstream& in );
  int    ReadLayerNum  ( ifstream&, string );
  double ReadUnitsValue( ifstream&, string );

  // output functions
  void   Debug_WireInput(string file="debug/wire_merge_input.tcl");
  void   Debug_Output(string file="debug/wire_merge_output.tcl");
};

//{{{
#if 0
class Design
{
  public:
    Design();
    ~Design();

    bool parseParameters(int argc, char** argv);
    void ReadAll();
    void ResultStat();
    virtual void Solve() = 0;

  protected:
		int m_numwires;         // number of wires (number of LG nodes)

    // *********************************
    //         Data structure
    // *********************************
		vector<myShape*>   m_Metals;
		map<oaShape*, int> MShape2Id;   // generate id for each myShape
    WireRTree          m_rtree;
		vector<bool>       vReadLayer;  // idendity which layer(s) to read

    // TIMER
    Timer m_wholeTimer;
    void StartWholeTimer();
    void EndWholeTimer();

	  // *********************************
		//          Parameters
	  // *********************************
    // parameters to access OpenAccess
    oaString library_;
    oaString cellName_;
    oaString viewName_;
    oaString outLibrary_;
    oaString outCellName_;
    oaString outViewName_;
    oaDesign* pInDesign_;  //input design
    oaDesign* pOutDesign_; //output design

    // parameters for algorithms
    int  algo_;
    int  polyid_;
    bool fast_;
    bool postfast_;  // try fast method in post-merging
    
    // parameters for debug
    bool readsmall_;  // whether to only read part of input layout 
    bool sim_output_; // simple output info

    // *****************************************
    //               Functions
    // *****************************************
		// for GDS
		void ReadGDSDesign();
    void AssignDefaultLayers();
    void TouchComponentCompute_RTree();
    void LayoutFracturing();

    // debug functions
    void Debug_Wires(const vector<mySuperShape*>, LayerInfo=LayerNum, string file="debug.tcl");
};
#endif
//}}}

#endif
