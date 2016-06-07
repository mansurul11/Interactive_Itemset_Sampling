/*
 *  Copyright (C) 2005 M.J. Zaki <zaki@cs.rpi.edu> Rensselaer Polytechnic Institute
 *  Written by parimi@cs.rpi.edu
 *  Updated by chaojv@cs.rpi.edu, alhasan@cs.rpi.edu, salems@cs.rpi.edu
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef _HELPER_FUNS_H_
#define _HELPER_FUNS_H_

#include <functional>
#include <ext/hash_set>
#include <cstring>
#define HASHNS __gnu_cxx

/** 
 * \struct eqstr
 * \brief function object which defines operator= for const char*
 */
struct eqstr
{
  /** 
   * \fn bool operator() (const char* s1, const char* s2) const
   * \brief returns true if s1 and s2 are the same sequence of characters
   */
  bool operator()(const char* s1, const char* s2) const {
    return strcmp(s1, s2) == 0;
  }
}; //end struct eqstr

/** 
 * \struct eqint
 * \brief function object which defines operator= for integer
 */
struct eqint
{
  /** 
   * \fn bool operator() (int i1, int s2) const
   * \brief returns true if i1 and i2 are the same integer
   */
  bool operator()(int i1, int i2) const {
    return i1 == i2;
  }
}; //end struct eqint

/**
 * \struct less_than
 * \brief function object for comparing two patterns for less-than
 */
template<class PAT>
struct less_than
{
   bool operator() (const PAT* p1, const PAT* p2) const {
     return (*p1 < *p2);
   }
};


template <typename PS>
struct PairCmp : std::binary_function<PS, PS, bool>{
  bool operator()(const PS& p, const PS& q) const {
    return ((p.first < q.first) || (p.first==q.first && p.second < q.second));
  }
};

/**
 * \struct hash_func
 */
template<class KEY>
struct hash_func
{ };

template <>
struct hash_func<std::string>:HASHNS::hash<const char*> {
size_t operator () (const std::string& x) const {
return this->HASHNS::hash<const char*>::operator () (x.c_str());
}
};

#endif
