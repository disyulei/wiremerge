#include <boost/algorithm/string.hpp>
#include "Design.h"
using namespace std;
using namespace bLib;


Design::Design()
{
  m_input       = "input.ascii";
  m_output      = "output.ascii";
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

  while (argc)
  {
    if ( strcmp(*argv, "-in") == 0 )
    {
      argc--, argv++;
      m_input = (*argv);
      argc--, argv++;
      continue;
    }
    if ( strcmp(*argv, "-out") == 0 )
    {
      argc--, argv++;
      m_output = (*argv);
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
Design::mergeWires()
{
  map<int,int>::iterator mitr;
  for (mitr=m_layer2Id.begin(); mitr!=m_layer2Id.end(); mitr++)
  {
    int lid = mitr->first;
    mergeWires( lid );
  }
}

void
Design::mergeWires(int lid)
//{{{
{
  int id = m_layer2Id[lid];
  if (id<0 || id>=m_Metals.size()) return;

  // Step 1: init RTree
  bLibRTree<myShape> m_rtree;
  for (int i=0; i<m_Metals[id].size(); i++) m_rtree.insert(m_Metals[id][i]);

  // Step 2: build up graph
  Graph G(m_Metals[id].size());
  for (int i=0; i<m_Metals[id].size(); i++)
  {
    int id1 = m_Metals[id][i]->getId();  assert(id1 == i);
    m_rtree.search( m_Metals[id][i]->x1(),
                    m_Metals[id][i]->y1(),
                    m_Metals[id][i]->x2(),
                    m_Metals[id][i]->y2());
    int size = bLib::bLibRTree<myShape>::s_searchResult.size();
    for (int j=0; j<size; j++)
    {
      myShape* adjshape = bLibRTree<myShape>::s_searchResult[j];
      int id2 = adjshape->getId();   if (id1 == id2) continue;

      bool bconnect = false;
      for (int k=0; k<m_Metals[id][id1]->m_realBoxes.size(); k++)
      for (int l=0; l<m_Metals[id][id2]->m_realBoxes.size(); l++)
      {
        myBox* box1 = m_Metals[id][id1]->m_realBoxes[k];
        myBox* box2 = m_Metals[id][id2]->m_realBoxes[l];
        if (false == box1->overlaps(box2, true)) continue;
        bconnect = true;
      } // for k, l

      if (true == bconnect) add_edge(id1, id2, G);
    } // for j
  } // for i

  // Step 3: ICC
  vector<int> component(num_vertices(G));
  int num = connected_components(G,&component[0]);
  m_mergeIds[id].clear();  m_mergeIds[id].resize(num);
  for (int i=0; i<m_Metals[id].size(); i++)
  {
    m_mergeIds[id][ component[i] ].push_back(i);
  }

  cout << "STAT| For layer "<<lid<<"\t num of wires before merge: " << m_Metals[id].size() << endl;
  cout << "STAT|                \t num of wires after  merge: " << num << endl;
}
//}}}


// ==== output functions
void
Design::outputAscii()
{
#ifdef _DEBUG_BEI
  cout << "DEBUG| OutputASCII()" << endl;
#endif
  string file = m_output;
  if (file.length() <= 0) file = "wireMerge_output.ascii";

  ofstream out;  out.open( file.c_str() );

  // Step 1: output header
  out<<"HEADER: 600"<<endl;
  out<<"BGNLIB: 2012, 9, 20, 2, 21, 41, 2012, 9, 20, 2, 21, 41"<<endl;
  out<<"LIBNAME: \"drc.db\""<<endl;
  out<<"UNITS: 0.001, 1e-9"<<endl;
  out<<"BGNSTR: 2012, 9, 20, 2, 21, 41, 2012, 9, 20, 2, 21, 41"<<endl;
  out<<"STRNAME: \"TOP\""<<endl;

  // Step 2: output each layers
  map<int,int>::iterator mitr;
  for (mitr=m_layer2Id.begin(); mitr!=m_layer2Id.end(); mitr++)
  {
    int lid = mitr->first;
    outputAscii(lid, out);
  }
  
  out<<"ENDSTR"<<endl;
  out<<"ENDLIB"<<endl;
	out.close();

  cout << "STAT| finish output to " << file << endl;
}


void
Design::outputAscii(int lid, ofstream& out)
{
  int id = m_layer2Id[lid];
  if (id<0 || id>=m_mergeIds.size()) return;

  for (int i=0; i<m_mergeIds[id].size(); i++)
  {
    int lsize = m_mergeIds[id][i].size();  assert(lsize > 0);
    if (1 == lsize)
    {
      int sid = m_mergeIds[id][i][0];
      myShape* pmyshape = m_Metals[id][sid];
      if (pmyshape->getPointNum() <= 0) continue;
      out<<"BOX"<<endl;
      out<<"LAYER: "<<lid<<endl;
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
    out<<"LAYER: "<<lid<<endl;
    out<<"BOXTYPE: 0"<<endl;
    out<<"XY: ";

    // merge into one polygon
    gtl::property_merge_90<int, int> pm;
    for (int j=0; j<m_mergeIds[id][i].size(); j++)
    {
      int sid = m_mergeIds[id][i][j];
      myShape* pmyshape = m_Metals[id][sid];
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
    for (int i=vsize-1; i>=0; i--)
    {
      out << vpoints[i].first << ", " << vpoints[i].second;
      if (i == 0) out << " ";
      else        out << ", ";
    }
    out << endl;

    out<<"ENDEL"<<endl;
  } // for i
}


// ====================================================
//               Reading Functions
// ====================================================
void
Design::readAll()
//{{{
{
  size_t found = m_input.rfind( ".ascii" );
  if (found == string::npos)
  {
    cout<<"ERROR| input should be a ascii file"<<endl;  exit(0);
  }

  readAscii(1);
  m_Metals.  clear();  m_Metals.  resize(m_layer2Id.size());
  m_mergeIds.clear();  m_mergeIds.resize(m_layer2Id.size());
  readAscii(2);

  // ==== debug
#if 0
  map<int,int>::iterator mitr;
  for (mitr=m_layer2Id.begin(); mitr!=m_layer2Id.end(); mitr++)
  {
    cout<<(*mitr).first<<" "<<(*mitr).second<<endl;
  } // for mitr
#endif
}
//}}}

double
Design::readBlock(int round, ifstream& in)
//{{{
{
  // parse information for block
  string str;
  if (false==readSearchUntil(in, str, "BOX", "BOUNDARY"))     return false;
  int layer = readLayerNum(in, "LAYER");       if (layer < 0) return false;
  if (false==readSearchUntil(in, str, "BOXTYPE", "DATATYPE")) return false;

  if (1>=round)
  {
    int size = m_layer2Id.size();
    if (m_layer2Id.find(layer)==m_layer2Id.end()) m_layer2Id[layer]=size;
    return true;
  }

  // read vpoints
  vector<myPoint> vpoints;  vpoints.clear();
  int xl = INT_MAX, yl = INT_MAX;
  int xh = INT_MIN, yh = INT_MIN;
  getline(in, str);  if (str.size() <= 0) return false;

  // old method to parse str
#if 0
  char* pch, chs[50000]; strcpy(chs, str.c_str());
  pch = strtok (chs, " :,");
  while (pch != NULL)
  {
    if (0 != isdigit(pch[0]) || '-' == pch[0])
    {
      int x = atoi( pch );
      pch = strtok (NULL, " :,");
      if (NULL == pch)
      {
        cout << "ERROR| in Design::ReadBlock()" << endl;
        cout << "chs = " << chs << endl;
        exit(-1);
      }
      int y = atoi( pch );
      
      if (m_ratio > 1.0 + 1e-4)
      {
        x = (int) (x / m_ratio);
        y = (int) (y / m_ratio);
      }
      vpoints.push_back( myPoint(x, y) );  // New point
      if (xl > x) xl = x;
      if (yl > y) yl = y;
      if (xh < x) xh = x;
      if (yh < y) yh = y;
    }
    pch = strtok (NULL, " :,");
  }
#endif
  // new method to parse str
#if 1
  string delimiters("\t :,");
  vector<string> parts;
  boost::split(parts, str, boost::is_any_of(delimiters), boost::token_compress_on);
  for (int i=1; i<parts.size(); i++)
  {
    int x = int ( atoi(parts[i].c_str()) / m_ratio );
    i++; if (i>=parts.size()) break;
    int y = int ( atoi(parts[i].c_str()) / m_ratio );

    vpoints.push_back( myPoint(x, y) );  // New point
    if (xl > x) xl = x;
    if (yl > y) yl = y;
    if (xh < x) xh = x;
    if (yh < y) yh = y;
  }
#endif
  if (vpoints.size() == 0) {return true;}
  vpoints.resize( vpoints.size()-1, true );

  myShape* pmyshape = new myShape(xl, yl, xh, yh);
  pmyshape->setPoints( vpoints );
  vector<myBox> vBoxes;
  bool bb = PTR::polygon2Rect(vpoints, vBoxes);
  if (true == bb) pmyshape->setRealBoxes(vBoxes);
  int lid = m_layer2Id[layer];
  pmyshape->setId( m_Metals[lid].size() );
  m_Metals[ lid ].push_back(pmyshape);

#if 0
  if (layer == layerMerge_) m_rtree.insert(pmyshape); 
#endif

  if (false == readSearchUntil( in, str, "ENDEL" )) return false;
  return true;
}
//}}}

void
Design::readAscii(int round)
//{{{
{
  ifstream in;  in.open(m_input.c_str());
  if (false == in.is_open())
  {
    cout<<"ERROR| cannot open file: "<<m_input<<endl; exit(0);
  }

  string str;
  readSearchUntil(in, str, "HEADER");
  readSearchUntil(in, str, "BGNLIB");
  readSearchUntil(in, str, "LIBNAME");
  double units = readUnitsValue(in, "UNITS");
  m_ratio = 0.001 / units;
  readSearchUntil(in, str, "BGNSTR");
  readSearchUntil(in, str, "STRNAME");

  bool bb = true;
  while (bb) bb = readBlock(round, in);
}
//}}}


double
Design::readUnitsValue( ifstream& in, const string& to_compare )
//{{{
{
  size_t found = string::npos;  string str;
  while (!in.eof())
  {
    getline( in, str );   if (in.eof()) return -1.0;
    found = str.find(to_compare);
    if (found != string::npos)
    {
      char* pch, chs[100];
      strcpy(chs, str.c_str());
      pch = strtok(chs,  " :,");
      pch = strtok(NULL, " :,");
      assert( isdigit( pch[0] ) != 0 );
      return atof( pch );
    } // find "UNITS"
  }
  return -1.0;
}
//}}}

int
Design::readLayerNum( ifstream& in, const string& to_compare )
//{{{
{
  size_t found = string::npos;
  string str;
  while ( ! in.eof() )
  {
    getline( in, str );  if ( in.eof() ) return -1;
    found = str.find( to_compare );
    if (found != string::npos)
    {
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


