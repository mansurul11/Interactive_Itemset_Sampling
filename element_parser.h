/** \file element_parser.h - class to parse a set of characters into the 
 * specialized element type */

#ifndef _ELEMENT_PARSER
#define _ELEMENT_PARSER
#include<sstream>
#include "helper_funs.h"

/** \brief Class represents a generic element_parser
 *
 * This is a dummy generic definition
 * It has to be partially specialized for each new element-type require
 */

template<typename P>
class element_parser {};

/** 
 * \brief Element parser class for parsing an integer, an specialization of element_parser class.
 */

template<>
class element_parser<int>
{

 public:

  typedef int OBJ_T; /**< element type */
  typedef int HASH_TYPE; // Input type for the hash function.
  typedef eqint COMP_FUNC; // Comparison function.

  /** \fn OBJ_T parse_element(char* word)
   * \brief parse characters in word to type OBJ_T
   * \param word input set of characters
   * return value is parsed element
   */
  static inline OBJ_T parse_element(char* word) { 
    return atoi(word);
  }

  static inline OBJ_T parse_element(string word) { 
    return atoi(word.c_str());
  }

  static const OBJ_T& convert(const int& i) {
 
    return i;
  }

  static bool notEq(const int& i1, const int& i2) {
    return !(i1 == i2);
  }

  static const HASH_TYPE& conv_hash_type(const OBJ_T& s) {
    return s;
  }


};//end clas element_parser<int>

/** 
 * \brief Element parser class for parsing a string, an specialization of element_parser class.
 */
template<>
class element_parser<std::string>
{

 public:

  typedef std::string OBJ_T; /**< element type */
  typedef const char* HASH_TYPE; // Input type for the hash function.
  typedef eqstr COMP_FUNC; // Comparison function.

  /** \fn OBJ_T parse_element(char* word)
   * \brief parse characters in word to type OBJ_T
   * \param word input set of characters
   * return value is parsed element
   */
  static inline OBJ_T parse_element(const char* word) { 
    return std::string(word);
  }

  static OBJ_T convert(const char* s) {
    return parse_element(s);
  }

  static OBJ_T convert(const int i) {
    std::ostringstream t_ss;
    t_ss << i;
 
    return t_ss.str();
  }

  static bool notEq(const std::string s1, const std::string s2) {
    return !(s1 == s2);
  }

  static HASH_TYPE conv_hash_type(const OBJ_T& s) {
    return s.c_str();
  }

};//end clas element_parser<std::string>

#endif
