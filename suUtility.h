#ifndef SU_UTILITY_H
#define SU_UTILITY_H

#include <algorithm>
#include <string>

/*!
    \brief Supplementary classes to be used mostly in combination with STL objects.

    This namespace contains elements that mostly depend on STL objects. It's intention is
    to provide an archive of small helper classes, functions and function objects that
    are closely tied to the C++ standard library. Some of the classes are written by me,
    others have been taken from books or the internet.

    \warning
    Changes and additions of functions that are project specific are not allowed here!
 */

namespace su
{

std::string trim_left (const std::string &a_str, const std::string &a_chars=" ");
std::string trim_right (const std::string &a_str, const std::string &a_chars=" ");
std::string trim(const std::string &a_str, const std::string &a_chars=" ");
std::string unquote(std::string &a_str);

template<typename TStr>
TStr to_upper(TStr a_sInput)
{
  TStr sOutput(a_sInput);
  int (*pf)(int) = toupper; 
  std::transform(sOutput.begin(), sOutput.end(), sOutput.begin(), pf); 

  return sOutput;
} 

}  // end of namespace string utils

#endif
