#ifndef _CELL_PARSER_H_
#define _CELL_PARSER_H_

#include "limbo/GdsReader.h"
#include "bLib/bLibShape.h"
#include <iostream>
#include <string>
using std::cout;
using std::endl;

class Design;

class CellDataBase: public GdsParser::GdsDataBase
{
public:
    CellDataBase(Design* design): m_design(design)
    {
    }

    template <typename ContainerType>
    void general_cbk(string const& ascii_record_type,
                                   string const& ascii_data_type,
                                   ContainerType const& data);

	///////////////////// required callbacks /////////////////////
	virtual void bit_array_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vBitArray);
	virtual void integer_2_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vInteger);
	virtual void integer_4_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vInteger);
	virtual void real_4_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<double> const& vFloat);
	virtual void real_8_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<double> const& vFloat);
	virtual void string_cbk(const char* ascii_record_type, const char* ascii_data_type, string const& str);
	virtual void begin_end_cbk(const char* ascii_record_type);

protected:
    Design*       m_design;

    std::string   m_type;
    int           m_layer;
    std::string   m_string;
    std::vector< std::pair<int, int> > m_datas;
};

#endif

