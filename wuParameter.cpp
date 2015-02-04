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
#include "wuParameter.h"

#include "suStringTokens.h"
#include "suUtility.h"


namespace wu
{

//---------------------------------------------------------------------------
// The statis instance
std::auto_ptr<Parameter> Parameter::m_pInst(0);

//---------------------------------------------------------------------------
Parameter& Parameter::GetInst()
{
  if (!m_pInst.get())
    m_pInst.reset(new Parameter());

  return *m_pInst;
}

//---------------------------------------------------------------------------
Parameter* Parameter::GetInstPtr()
{
  if (!m_pInst.get())
    m_pInst.reset(new Parameter());

  return m_pInst.get();
}

//---------------------------------------------------------------------------
Parameter::Parameter()
  :m_sCmdLine()
  ,m_Param()
{
  m_sCmdLine = GetCommandLine();
  
  su::StringTokens<string_type> par(m_sCmdLine, _T("/"));
  int iNum = par.Count();
  
  su::StringTokens<string_type> subpar;

  // don't split the path and dont add an additional argument
  m_Param[par[0]] = _T("");

  // store options in capital letters
  for (int i=1; i<iNum; ++i)
  {
    subpar.Tokenize(par[i], _T(" :"));
    m_Param[su::to_upper(subpar[0])] = (subpar.Count()>=2) ? subpar[1] : _T("");
  }
}

//---------------------------------------------------------------------------
const Parameter::string_type& Parameter::GetCmdLine() const
{
  return m_sCmdLine;
}

//---------------------------------------------------------------------------
unsigned Parameter::GetArgc() const
{
  return (unsigned)m_Param.size();
}

//---------------------------------------------------------------------------
/** \brief Check whether a given Parameter was present in the command line.
*/
bool Parameter::IsOpt(const string_type &a_sArg) const
{
  map_type::const_iterator item = m_Param.find(a_sArg);
  return (item!=m_Param.end());
}

//---------------------------------------------------------------------------
const Parameter::string_type& Parameter::GetOpt(const string_type &a_sOpt) const
{
  static const string_type sEmpty;
  string_type sOpt = su::to_upper(a_sOpt);

  map_type::const_iterator item = m_Param.find(sOpt);
  return (item!=m_Param.end()) ? item->second : sEmpty; 
}

}
