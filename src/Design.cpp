#include "Design.h"
using namespace std;
using namespace bLib;

Design::Design()
//{{{
{
  library_     = "MX_Benchmark1_blind_partial.ascii";
  layerMerge_  = 10;
  layerMax_    = 500;

  ratio_       = 1.0;

  m_Metals.clear(); 
  m_Metals.resize( layerMax_ );
}
//}}}

Design::~Design()
{
  for (int i=0; i<m_Metals.size(); i++)
  for (int j=0; j<m_Metals[i].size(); j++)
    delete m_Metals[i][j];
}

bool
Design::parseParameters(int argc, char** argv)
//{{{
{
  argc--, argv++; // parse "main"
#ifdef _DEBUG_BEI
  cout << "Parameters: ";
	for (int i=0; i<argc; i++) cout << argv[i] << " ";
	cout << endl;
#endif

  while (argc) {
    // parameters for input/output setting
    if ( strcmp(*argv, "-inlib") == 0 ) {
      argc--, argv++;
      library_ = (*argv);
      argc--, argv++;
      continue;
    }
    if ( strcmp(*argv, "-layer") == 0 ) {
      argc--, argv++;
      layerMerge_ = atoi(*argv);
      argc--, argv++;
      continue;
    }

		cout << "ERROR| can NOT parse: " << *argv << endl;
		return false;
  }
  return true;
}
//}}}

void
Design::readAll()
{
#ifdef _DEBUG_BEI
  cout<<"DEBUG| readAll() "<<endl;
#endif
  
  size_t found = library_.rfind( ".ascii" );
  if (found == string::npos) {
    cout << "ERROR| is not a ascii file" << endl;
    exit(0);
  }

  ReadASCII();
}

void
Design::ReadASCII()
//{{{
{
#ifdef _DEBUG_BEI
  cout<<"DEBUG| ReadASCII() "<<endl;
#endif

  ifstream in;  in.open(library_.c_str());
  if (NULL == in) {
    cout << "ERROR| cannot open file: " << library_ << endl;
    exit(0);
  }

  ReadSearchUntil(in, "HEADER");
  ReadSearchUntil(in, "BGNLIB");
  ReadSearchUntil(in, "LIBNAME");
  double units = ReadUnitsValue(in, "UNITS");
  ratio_ = 0.001 / units;
#ifdef _DEBUG_BEI
  cout << "DEBUG| units = " << units << endl;
  cout << "DEBUG| ratio_ = " << ratio_ << endl;
#endif
  ReadSearchUntil(in, "BGNSTR");
  ReadSearchUntil(in, "STRNAME");

  bool bb = true;
  while (bb) {
    bb = ReadBlock(in);
  }

  Debug_WireInput();
}
//}}}

bool
Design::ReadSearchUntil( ifstream& in, string str1, string str2 )
//{{{
{
  size_t found = string::npos;
  string str;
  while ( ! in.eof() ) {
    getline( in, str );
    if ( in.eof() ) return false;
    found = str.find( str1 );
    if (found != string::npos) return true;
    if (str2.length()==0) continue;
    found = str.find( str2 );
    if (found != string::npos) return true;
  }
  return false;
}
//}}}

double
Design::ReadUnitsValue( ifstream& in, string to_compare )
//{{{
{
  size_t found = string::npos;
  string str;
  while ( ! in.eof() ) {
    getline( in, str );
    if ( in.eof() ) return -1;
    found = str.find( to_compare );
    if (found != string::npos) {
      char* pch, chs[100];
      strcpy(chs, str.c_str());
      pch = strtok(chs,  " :,");
      pch = strtok(NULL, " :,");
      assert( isdigit( pch[0] ) != 0 );
      return atof( pch );
    } // find "UNITS"
  }
  return -1;
}
//}}}

int
Design::ReadLayerNum( ifstream& in, string to_compare )
//{{{
{
  size_t found = string::npos;
  string str;
  while ( ! in.eof() ) {
    getline( in, str );
    if ( in.eof() ) return -1;
    found = str.find( to_compare );
    if (found != string::npos) {
      char* pch, chs[100];
      strcpy(chs, str.c_str());
      pch = strtok(chs,  " :,");
      pch = strtok(NULL, " :,");
      assert( isdigit( pch[0] ) != 0 );
      return atoi( pch );
    } // find "LAYER"
  }
  return -1;
}
//}}}

bool
Design::ReadBlock( ifstream& in )
//{{{
{
  // parse information for block
  if (false == ReadSearchUntil( in, "BOX", "BOUNDARY" )) return false;
  int layer =  ReadLayerNum(in, "LAYER");
  if (layer < 0)          return false;
  if (layer >= layerMax_) return false;
  if (false == ReadSearchUntil( in, "BOXTYPE", "DATATYPE" )) return false;

  vector<myPoint> vpoints;  vpoints.clear();
  int xl = INT_MAX, yl = INT_MAX;
  int xh = INT_MIN, yh = INT_MIN;
  string str; getline(in, str);

  if (str.size() <= 0) return false;
  char* pch, chs[20000];
  strcpy(chs, str.c_str());
  pch = strtok (chs, " :,");
  while (pch != NULL)
  {
    if (0 != isdigit(pch[0]) || '-' == pch[0]) {
    int x = atoi( pch );
      pch = strtok (NULL, " :,");
     if (NULL == pch) {
        cout << "ERROR| in Design::ReadBlock()" << endl;
        cout << "chs = " << chs << endl;
        exit(-1);
      }
      int y = atoi( pch );
      
      if (ratio_ > 1.0 + 1e-4) {
        x = (int) (x / ratio_);
        y = (int) (y / ratio_);
      }
      // New point
      vpoints.push_back( myPoint(x, y) );
      if (xl > x) xl = x;
      if (yl > y) yl = y;
      if (xh < x) xh = x;
      if (yh < y) yh = y;
    }
    pch = strtok (NULL, " :,");
  }
  if (vpoints.size() == 0) {return true;}
  vpoints.resize( vpoints.size()-1, true );

  myShape* pmyshape = new myShape(xl, yl, xh, yh);
  pmyshape->setPoints( vpoints );
  pmyshape->setId( m_Metals[layer].size() );

  vector<myBox> vBoxes;
  bool bb = PTR::polygon2Rect(vpoints, vBoxes);
  if (true == bb) pmyshape->setRealBoxes(vBoxes);

  m_Metals[layer].push_back(pmyshape);
#if 1
  if (layer == layerMerge_) m_rtree.insert(pmyshape); 
#endif

  if (false == ReadSearchUntil( in, "ENDEL" )) return false;
  return true;
}
//}}}

void
Design::mergeWires()
{
#ifdef _DEBUG_BEI
  cout << "DEBUG| Design::mergeWires()" << endl;
#endif
  TouchComponentCompute_RTree();
  Debug_Output();

  OutputASCII();            // for each component, output the boundary points
}

void
Design::TouchComponentCompute_RTree()
/*{{{*/
{
#ifdef _DEBUG_BEI
  cout << "DEBUG| Design::TouchComponentCompute_RTree()" << endl;
#endif

  Graph G(m_Metals[layerMerge_].size());

  for (int i=0; i<m_Metals[layerMerge_].size(); i++)
  {
    //printf ("TouchComponentCompute Process: %.2f%\n", (double)i/(double)m_Metals[layerMerge_].size()*100);

    int sourceid = m_Metals[layerMerge_][i]->getId();
    assert( sourceid == i );

    m_rtree.search( m_Metals[layerMerge_][i]->x1(),
                    m_Metals[layerMerge_][i]->y1(),
                    m_Metals[layerMerge_][i]->x2(),
                    m_Metals[layerMerge_][i]->y2());
    int size = bLib::bLibRTree<myShape>::s_searchResult.size();
    for (int j=0; j<size; j++)
    {
      myShape* adjshape = bLibRTree<myShape>::s_searchResult[j];
      int sinkid = adjshape->getId();
      if (sourceid == sinkid) continue;

      bool bconnect = false;
      for (int k=0; k<m_Metals[layerMerge_][sourceid]->m_realBoxes.size(); k++)
      {
        myBox* box1 = m_Metals[layerMerge_][sourceid]->m_realBoxes[k];
        for (int l=0; l<m_Metals[layerMerge_][sinkid]->m_realBoxes.size(); l++)
        {
          myBox* box2 = m_Metals[layerMerge_][sinkid]->m_realBoxes[l];
          if (false == box1->overlaps(box2, true)) continue;
          bconnect = true;
        } // for l
      } // for k

      if (true == bconnect) {
        add_edge(sourceid, sinkid, G);
      }
    } // for j
  } // for i
  //printf ("TouchComponentCompute Process:   100%    \r\n");

  vector<int> component(num_vertices(G));
  int num = connected_components(G,&component[0]);

  m_MergedWireIDs.clear();
  m_MergedWireIDs.resize( num );
  for (int i = 0; i < m_Metals[layerMerge_].size(); i++)
  {
    int id = component[i];
    m_MergedWireIDs[id].push_back(i);
  }

  #ifdef _DEBUG_BEI
    printf ("DEBUG| num of wires to merge is %d, num of wires after merge is %d\n", m_Metals[layerMerge_].size(), num);
  #endif
  cout << "STAT| num of wires before merge: " << m_Metals[layerMerge_].size() << endl;
  cout << "STAT| num of wires after  merge: " << num << endl;
}
/*}}}*/


// =========================================================
//   output all input polygons
// =========================================================
void
Design::Debug_WireInput(string file)
//{{{
{
#ifdef _DEBUG_BEI
  cout << "DEBUG| Design::Debug_WireInput(), output tcl file is 'wire_merge_input.tcl'" << endl;
#endif

  ofstream out;
  out.open( file.c_str() );
	out << "set L [layout create]" << endl;
	out << "$L create cell debug" << endl;
  for (int idx=0; idx<layerMax_; idx++)
	  out << "$L create layer " << idx << endl; 

  for (int i=0; i<m_Metals[layerMerge_].size(); i++)
  {
    myShape* pmyshape = m_Metals[layerMerge_][i];
		//out << "$L create polygon debug " << 0 << " ";  
    //oaBox* poabox = pmyshape->m_realBox;
		//out	<< poabox->x1() << " " << poabox->y1() << " " << poabox->x2() << " " << poabox->y2()  << endl;

    for (int j=0; j<pmyshape->m_realBoxes.size(); j++)
    {
      myBox* pbox = pmyshape->m_realBoxes[j];
		  out << "$L create polygon debug " << layerMerge_ << " ";  
		  out	<< pbox->x1() << " " << pbox->y1() << " " << pbox->x2() << " " << pbox->y2()  << endl;
    } // for j
  } // for i

	out << "$L gdsout debug/wire_merge_input.gds\n" << endl;
	out.close();
}
//}}}

void
Design::Debug_Output(string file)
//{{{
{
#ifdef _DEBUG_BEI
  cout << "DEBUG| Design::Debug_Output()" << endl;
#endif

  ofstream out;
  out.open( file.c_str() );
	out << "set L [layout create]" << endl;
	out << "$L create cell debug" << endl;
	out << "$L create layer " << layerMerge_ << endl; 
	out << "$L create layer " << layerMerge_+1 << endl; 

  for (int i=0; i<m_MergedWireIDs.size(); i++) {
    int lsize = m_MergedWireIDs[i].size();
    assert(lsize > 0);
    if (1 == lsize)
    {
      int id = m_MergedWireIDs[i][0];
      myShape* pmyshape = m_Metals[layerMerge_][id];
      out << "$L create polygon debug " << layerMerge_ << " ";  
      for (int j=0; j<pmyshape->getPointNum(); j++)
      {
        out << pmyshape->getPointX(j) << ", " << pmyshape->getPointY(j) << ", ";
      }
      out << endl;
      continue;
    }

    // merge into one polygon
    gtl::property_merge_90<int, int> pm;
    for (int j=0; j<m_MergedWireIDs[i].size(); j++)
    {
      int id = m_MergedWireIDs[i][j];
      myShape* pmyshape = m_Metals[layerMerge_][id];
      for (int k=0; k<pmyshape->m_realBoxes.size(); k++)
      {
        myBox* poabox = pmyshape->m_realBoxes[k];
        pm.insert( gtl::rectangle_data<int>(poabox->x1(), poabox->y1(), poabox->x2(), poabox->y2()), 0 );
      } // for k
    } // for j
    map< set<int>, gtl::polygon_90_set_data<int> > result;
    pm.merge(result);
    set<int> settmp; settmp.insert(0);
    map< set<int>, gtl::polygon_90_set_data<int> >::iterator itr = result.find(settmp);
    gtl::polygon_90_set_data<int> polyset = itr->second;
    vector<Polygon> output;
    polyset.get_polygons(output);
    Polygon& poly = output[0];

    // output the polygon
		out << "$L create polygon debug " << layerMerge_+1 << " ";  
    Polygon::iterator_type poly_itr = poly.begin(), poly_end = poly.end();
    for (; poly_itr != poly_end; poly_itr++) {
      int x = gtl::x(*poly_itr);
      int y = gtl::y(*poly_itr);
      out << x << " " << y << " ";
    }
    out << endl;

  } // for i

	out << "$L gdsout debug/wire_merge_output.gds\n" << endl;
	out.close();
}
//}}}


// output old ascii format
void
Design::OutputASCII()
//{{{
{
#ifdef _DEBUG_BEI
  cout << "DEBUG| OutputASCII()" << endl;
#endif
  string file = library_;
  size_t found = file.rfind( ".ascii" );
  if (found == string::npos)
  {
    cout << "ERROR| in Design::OutputASCII(), file is not a ascii file" << endl;
    exit(0);
  }
  file.replace(found, 0, "_merged");

  ofstream out;
  out.open( file.c_str() );

  // output header
  out<<"HEADER: 600"<<endl;
  out<<"BGNLIB: 2012, 9, 20, 2, 21, 41, 2012, 9, 20, 2, 21, 41"<<endl;
  out<<"LIBNAME: \"drc.db\""<<endl;
  out<<"UNITS: 0.001, 1e-9"<<endl;
  out<<"BGNSTR: 2012, 9, 20, 2, 21, 41, 2012, 9, 20, 2, 21, 41"<<endl;
  out<<"STRNAME: \"TOP\""<<endl;

  // output other layers
  for (int idx=0; idx<m_Metals.size(); idx++) {
    if (idx == layerMerge_) continue;
    for (int j=0; j<m_Metals[idx].size(); j++) {
      out<<"BOX"<<endl;
      out<<"LAYER: "<<idx<<endl;
      out<<"BOXTYPE: 0"<<endl;
      out<<"XY: ";
      myShape* pmyshape = m_Metals[idx][j];
      for (int j=0; j<pmyshape->getPointNum(); j++)
      {
        out << pmyshape->getPointX(j) << ", " << pmyshape->getPointY(j) << ", ";
      }
      out << pmyshape->getPointX(0) << ", " << pmyshape->getPointY(0) << endl;
      out<<"ENDEL"<<endl;
    } // for j
  } // for idx

  // output each wire
  for (int i=0; i<m_MergedWireIDs.size(); i++)
  {
    int lsize = m_MergedWireIDs[i].size();
    assert(lsize > 0);
    if (1 == lsize) {
      int id = m_MergedWireIDs[i][0];
      myShape* pmyshape = m_Metals[layerMerge_][id];
      if (pmyshape->getPointNum() <= 0) continue;
      out<<"BOX"<<endl;
      out<<"LAYER: "<<layerMerge_<<endl;
      out<<"BOXTYPE: 0"<<endl;
      out<<"XY: ";
      for (int j=0; j<pmyshape->getPointNum(); j++)
      {
        out << pmyshape->getPointX(j) << ", " << pmyshape->getPointY(j) << ", ";
      }
      out << pmyshape->getPointX(0) << ", " << pmyshape->getPointY(0) << endl;
      out<<"ENDEL"<<endl;
      continue;
    }

    out<<"BOX"<<endl;
    out<<"LAYER: "<<layerMerge_<<endl;
    out<<"BOXTYPE: 0"<<endl;
    out<<"XY: ";
    // merge into one polygon
    gtl::property_merge_90<int, int> pm;
    for (int j=0; j<m_MergedWireIDs[i].size(); j++) {
      int id = m_MergedWireIDs[i][j];
      myShape* pmyshape = m_Metals[layerMerge_][id];
      for (int k=0; k<pmyshape->m_realBoxes.size(); k++) {
        myBox* poabox = pmyshape->m_realBoxes[k];
        pm.insert( gtl::rectangle_data<int>(poabox->x1(), poabox->y1(), poabox->x2(), poabox->y2()), 0 );
      } // for k
    } // for j
    map< set<int>, gtl::polygon_90_set_data<int> > result;
    pm.merge(result);
    set<int> settmp; settmp.insert(0);
    map< set<int>, gtl::polygon_90_set_data<int> >::iterator itr = result.find(settmp);
    gtl::polygon_90_set_data<int> polyset = itr->second;
    vector<Polygon> output;
    polyset.get_polygons(output);
    Polygon& poly = output[0];

    // output the polygon
    // NOTE: after merging, the polygon is scaned clockwise
    //       should output the polygon ANTI-CLOCKWISE
    vector<pair<int, int> > vpoints;
    Polygon::iterator_type poly_itr = poly.begin(), poly_end = poly.end();
    for (; poly_itr != poly_end; poly_itr++) {
      int x = gtl::x(*poly_itr);
      int y = gtl::y(*poly_itr);
      vpoints.push_back( make_pair(x, y) );
    }
    poly_itr = poly.begin();
    int x = gtl::x(*poly_itr);
    int y = gtl::y(*poly_itr);
    vpoints.push_back( make_pair(x, y) );

    int vsize = vpoints.size();
    for (int i=vsize-1; i>=0; i--) {
      out << vpoints[i].first << ", " << vpoints[i].second;
      if (i == 0) out << " ";
      else        out << ", ";
    }
    out << endl;

    out<<"ENDEL"<<endl;
  } // for i

  out<<"ENDSTR"<<endl;
  out<<"ENDLIB"<<endl;
	out.close();

  cout << "STAT| finish output to " << file << endl;
}
//}}}


// output new ascii format used in DAC'12
void
Design::OutputASCII_new()
//{{{
{
#ifdef _DEBUG_BEI
  cout << "DEBUG| OutputASCII_new()" << endl;
#endif
  string file = library_;
  size_t found = file.rfind( ".ascii" );
  if (found == string::npos) {
    cout << "ERROR| in Design::OutputASCII_new(), file is not a ascii file" << endl;
    exit(0);
  }
  file.replace(found, 0, "_merged");

  ofstream out;
  out.open( file.c_str() );

  // output header
  out<<"HEADER: 600"<<endl;
  out<<"BGNLIB: 2012, 9, 20, 2, 21, 41, 2012, 9, 20, 2, 21, 41"<<endl;
  out<<"LIBNAME: \"drc.db\""<<endl;
  out<<"UNITS: 0.001, 1e-9"<<endl;
  out<<"BGNSTR: 2012, 9, 20, 2, 21, 41, 2012, 9, 20, 2, 21, 41"<<endl;
  out<<"STRNAME: \"TOP\""<<endl;

  // output other layers
  for (int idx=0; idx<m_Metals.size(); idx++)
  {
    if (idx == layerMerge_) continue;
    for (int j=0; j<m_Metals[idx].size(); j++)
    {
      out<<"BOX"<<endl;
      out<<"LAYER: "<<idx<<endl;
      out<<"BOXTYPE: 0"<<endl;
      out<<"XY: ";
      myShape* pmyshape = m_Metals[idx][j];
      out << pmyshape->getPointNum() + 1 << "  ";   // here is the only new thing
      for (int j=0; j<pmyshape->getPointNum(); j++)
      {
        out << pmyshape->getPointX(j) << ", " << pmyshape->getPointY(j) << ", ";
      }
      out << pmyshape->getPointX(0) << ", " << pmyshape->getPointY(0) << endl;
      out<<"ENDEL"<<endl;
    } // for j
  } // for idx

  // output each wire
  for (int i=0; i<m_MergedWireIDs.size(); i++)
  {
    int lsize = m_MergedWireIDs[i].size();
    assert(lsize > 0);
    if (1 == lsize)
    {
      int id = m_MergedWireIDs[i][0];
      myShape* pmyshape = m_Metals[layerMerge_][id];
      if (pmyshape->getPointNum() <= 0) continue;
      out<<"BOX"<<endl;
      out<<"LAYER: "<<layerMerge_<<endl;
      out<<"BOXTYPE: 0"<<endl;
      out<<"XY: ";
      out << pmyshape->getPointNum() + 1 << "  ";    // the only new thing
      for (int j=0; j<pmyshape->getPointNum(); j++)
      {
        out << pmyshape->getPointX(j) << ", " << pmyshape->getPointY(j) << ", ";
      }
      out << pmyshape->getPointX(0) << ", " << pmyshape->getPointY(0) << endl;
      out<<"ENDEL"<<endl;
      continue;
    }

    out<<"BOX"<<endl;
    out<<"LAYER: "<<layerMerge_<<endl;
    out<<"BOXTYPE: 0"<<endl;
    out<<"XY: ";

    // merge into one polygon
    gtl::property_merge_90<int, int> pm;
    for (int j=0; j<m_MergedWireIDs[i].size(); j++)
    {
      int id = m_MergedWireIDs[i][j];
      myShape* pmyshape = m_Metals[layerMerge_][id];
      for (int k=0; k<pmyshape->m_realBoxes.size(); k++)
      {
        myBox* poabox = pmyshape->m_realBoxes[k];
        pm.insert( gtl::rectangle_data<int>(poabox->x1(), poabox->y1(), poabox->x2(), poabox->y2()), 0 );
      } // for k
    } // for j

    map< set<int>, gtl::polygon_90_set_data<int> > result;
    pm.merge(result);
    set<int> settmp; settmp.insert(0);
    map< set<int>, gtl::polygon_90_set_data<int> >::iterator itr = result.find(settmp);
    gtl::polygon_90_set_data<int> polyset = itr->second;
    vector<Polygon> output;
    polyset.get_polygons(output);
    Polygon& poly = output[0];

    // output the polygon
    // NOTE: after merging, the polygon is scaned clockwise
    //       should output the polygon ANTI-CLOCKWISE
    vector<pair<int, int> > vpoints;
    Polygon::iterator_type poly_itr = poly.begin(), poly_end = poly.end();
    for (; poly_itr != poly_end; poly_itr++)
    {
      int x = gtl::x(*poly_itr);
      int y = gtl::y(*poly_itr);
      vpoints.push_back( make_pair(x, y) );
    }
    poly_itr = poly.begin();
    int x = gtl::x(*poly_itr);
    int y = gtl::y(*poly_itr);
    vpoints.push_back( make_pair(x, y) );

    int vsize = vpoints.size();
    out << vsize << "  ";   // here is the only new thing
    for (int i=vsize-1; i>=0; i--)
    {
      out << vpoints[i].first << ", " << vpoints[i].second;
      if (i == 0) out << " ";
      else        out << ", ";
    }
    out << endl;

    out<<"ENDEL"<<endl;
  } // for i

  out<<"ENDSTR"<<endl;
  out<<"ENDLIB"<<endl;
	out.close();

  cout << "STAT| finish output to " << file << endl;
}
//}}}



