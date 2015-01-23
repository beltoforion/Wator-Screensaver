/*
  WorldOfWator
  Copyright 2009 Ingo Berg

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef WU_PARAMETER_H
#define WU_PARAMETER_H

#include "StdAfx.h"

#include <memory>
#include <map>
#include <string>


namespace wu
{

//-------------------------------------------------------------------------------
class Parameter
{
    friend class std::auto_ptr<Parameter>;

public:
    typedef TCHAR char_type;
    typedef std::basic_string<char_type> string_type;
  
private:
    typedef std::map<string_type, string_type> map_type;

public:
    static Parameter& GetInst();
    static Parameter* GetInstPtr();

    const string_type& GetCmdLine() const;
    unsigned GetArgc() const;
    bool IsOpt(const string_type& a_sParam) const;
    const string_type& GetOpt(const string_type &a_sOpt) const;

private:
    static std::auto_ptr<Parameter> m_pInst;
    string_type m_sCmdLine;
    map_type m_Param;

    Parameter();
    Parameter(const Parameter&) {};
   ~Parameter() {};
};


}  // end of namespace

#endif