#include "Parser.h"
#include "Design.h"
using namespace std;

template <typename ContainerType>
void CellDataBase::general_cbk(string const& ascii_record_type,
                               string const& ascii_data_type,
                               ContainerType const& data)
{
	if (ascii_record_type == "UNITS")
	{
        m_type = "TOP";
	}
	else if (ascii_record_type == "ENDEL")
	{
        if ("BOUNDARY" == m_type)
        {
            //vpoints
            vector<bLib::bPoint> vpoints;  vpoints.clear();
            int xl = INT_MAX, yl = INT_MAX;
            int xh = INT_MIN, yh = INT_MIN;
            for (int i=0; i<m_datas.size(); i++)
            {
                int x = m_datas[i].first;
                int y = m_datas[i].second;
                vpoints.push_back( bLib::bPoint(x, y) );  // New points
                if (xl > x) xl = x;
                if (yl > y) yl = y;
                if (xh < x) xh = x;
                if (yh < y) yh = y;
            }
            if (vpoints.size() == 0)  {return;}
            vpoints.resize( vpoints.size()-1, true );
            bLib::bShape* pmyshape = new bLib::bShape(xl, yl, xh, yh);
            pmyshape->setPoints( vpoints );
            // points to a set of boxes
            vector<bLib::bBox> bboxes;
            bool bb = bLib::PTR::polygon2Rect(vpoints, bboxes, 1); // flag = 1 vertical
            if (true==bb) pmyshape->setRealBoxes(bboxes);
            
            int lid = m_design->getLayerId(m_layer);
            pmyshape->setId(lid);

            m_design->addMetalShape(pmyshape, lid);
        }

        if ("TEXT" == m_type)
        {
            size_t found = m_string.find('&');
            if (found == string::npos) m_design->addText(m_datas[0]);
        }
        m_type = "TOP";
	}
	else if (ascii_record_type == "BOUNDARY" || ascii_record_type == "BOX")
	{
        m_type = "BOUNDARY";
	}
	else if (ascii_record_type == "TEXT")
	{
        m_type = "TEXT";
	}
	else if (ascii_record_type == "LAYER")
	{
        m_layer = data[0];
	}
	else if (ascii_record_type == "STRING")
	{
        m_string.assign(data.begin(), data.end());
	}
	else if (ascii_record_type == "XY")
	{
        m_datas.clear();
        for (int i=0; i<data.size(); i+=2)
        {
            pair<int, int> xydata (data[i], data[i+1]);
            m_datas.push_back(xydata);
        }
	}
}



void
CellDataBase::bit_array_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vBitArray)
{
    //cout << __func__ << endl;
	this->general_cbk(ascii_record_type, ascii_data_type, vBitArray);
}
void
CellDataBase::integer_2_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vInteger)
{
    //cout << __func__ << endl;
	this->general_cbk(ascii_record_type, ascii_data_type, vInteger);
}
void
CellDataBase::integer_4_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vInteger)
{
    //cout << __func__ << endl;
	this->general_cbk(ascii_record_type, ascii_data_type, vInteger);
}
void
CellDataBase::real_4_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<double> const& vFloat) 
{
    //cout << __func__ << endl;
	this->general_cbk(ascii_record_type, ascii_data_type, vFloat);
}
void
CellDataBase::real_8_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<double> const& vFloat) 
{
    //cout << __func__ << endl;
	this->general_cbk(ascii_record_type, ascii_data_type, vFloat);
}
void
CellDataBase::string_cbk(const char* ascii_record_type, const char* ascii_data_type, string const& str) 
{
    //cout << __func__ << endl;
	this->general_cbk(ascii_record_type, ascii_data_type, str);
}
void
CellDataBase::begin_end_cbk(const char* ascii_record_type)
{
    //cout << __func__ << endl;
	this->general_cbk(ascii_record_type, "", vector<int>(0));
}


