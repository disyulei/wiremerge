#ifndef _B_LIB_BASE_H_
#define _B_LIB_BASE_H_

// ============================================================== //
//   Library for some common functions
//
//   Author        : Bei Yu
//   Last update   : 06/2014
// ============================================================== //

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <cstdio>
#include <cmath>            // ceilf(), floorf()
#include <fstream>
#include <string>
#include <iostream>
#include <sys/resource.h>   // timer()
#include <sys/time.h>       // timer()

namespace bLib
{
  // ==========================================================
  //          Trim left & right space of one string
  // ==========================================================
  // trim from start
  inline std::string & ltrim(std::string &s)
  {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
  }

  // trim from end
  inline std::string & rtrim(std::string &s)
  {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
  }
  
  // trim from both ends
  inline std::string & trim(std::string &s)
  {
    return ltrim(rtrim(s));
  }


  // ============================================================
  //                   Check file exists
  // ============================================================
  inline bool isFileExist(const char* filename)
  {
    std::ifstream infile(filename);
    return infile.good();
  }


  // ============================================================
  //                   Read File Functions
  // ============================================================
  // read from ifstream in, search target1 & target2,
  // if found, return true, and the whole line is stored in "str_line"
  // if reach EOF, return false
  inline bool readSearchUntil(std::ifstream& in, std::string & str_line, const std::string target1, const std::string target2="")
  {
    while (!in.eof())
    {
      getline( in, str_line );  if (in.eof())          return false;
      if (std::string::npos != str_line.find(target1)) return true;
      if (target2.length() <= 0) continue;
      if (std::string::npos != str_line.find(target2)) return true;
    } // while
    return false;
  }


  // =================================================================
  //                  Parse string
  // =================================================================
  // given path in str_path, output the design name
  // for example: with input "path1/path2/sparc_exu_alu", output "sparc_exu_alu"
  inline std::string parsePath2Name(std::string str_path)
  {
    size_t pos = str_path.rfind('/');
    if (pos == std::string::npos) return str_path;
  
    return str_path.substr(pos+1);
  }

  // trim filename suffix
  inline std::string trimSuffix(std::string str)
  {
    size_t pos = str.rfind('.');
    return str.substr(0, pos);
  }


  // ============================================================
  //               Numerical functions
  // ============================================================
  inline bool isInteger(float fvalue)
  {
    if ( ceilf (fvalue) != fvalue ) return false;
    if ( floorf(fvalue) != fvalue ) return false;
    return true;
  }

  // ==============================================
  //             timer functions
  // ==============================================
  inline double timerSelf()
  {
    rusage r;
    getrusage(RUSAGE_SELF, &r);
    return (double)(r.ru_utime.tv_sec+r.ru_utime.tv_usec/(double)1000000);
  }
  
  inline double timerChild()
  {
    rusage r;
    getrusage(RUSAGE_CHILDREN, &r);
    return (double)(r.ru_utime.tv_sec+r.ru_utime.tv_usec/(double)1000000);
  }
  
  inline double timer()
  {
    return timerSelf() + timerChild();
  }
}

/*
// ==== Implementation Log:
//
// 06/2014: new function readSearchUntil(), added target2
//
// 06/2014: add function trimSuffix()
//
// 05/2014: add namespace bLib, discard class
//
// 05/2014: new function isFileExist()
//
// 05/2014: check fstream with is_open() function
//
*/


#endif


