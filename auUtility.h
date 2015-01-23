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

#ifndef AU_UTILITY_H
#define AU_UTILITY_H

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <typeinfo>

//-------------------------------------------------------------------------------------------
//
//  Wator Screensaver
//  (C) 2005 Ingo Berg
//
//  ingo_berg {at} gmx {dot} de
//
//-------------------------------------------------------------------------------------------

#define FAIL(name)            \
        {                     \
          bool name = false;  \
          assert(name);       \
        }          


namespace au
{

//-------------------------------------------------------------------------------------------
int rnum(int min, int max);
int rnum(int max);

//-------------------------------------------------------------------------------------------
// the following is taken from a news posting by Andrei Alexandrescu, found in google 
// groups
template <typename T>
void mem2zero(T& obj)
{
  // check for constructors
  // will generate compile-time errors
  // if the type has cdtors
  union cdtor_checker
  {
    T test;
    char dummy;
  };

  // check for virtuals
  // will generate a link-time error
  // if the object has virtuals
  extern T* test_virtuals();
  assert(typeid(test_virtuals()) == typeid(&obj));

  // we can now safely memset
  memset(&obj, 0, sizeof(obj));
} 

} // namespace utils

#endif // AU_UTILITY_H
