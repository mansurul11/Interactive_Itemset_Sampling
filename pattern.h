#ifndef _PATTERN_H_
#define _PATTERN_H_

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <cassert>

using namespace std;

template <typename V_T>
class Pattern;

template <typename V_T>
ostream& operator<< (ostream& ostr, const Pattern<V_T>& );

// For itemset pattern V_T denotes the datatype
template <typename V_T>
class Pattern 
{
  friend ostream& operator<< <>(ostream& osr, const Pattern<V_T>&);

  public:
    typedef Pattern<V_T> PAT;
    typedef typename set<V_T>::const_iterator CVIT;
    typedef pair<CVIT, CVIT> CIT_PAIR;
    typedef typename set<V_T>::iterator VIT;
    typedef V_T VERTEX_T;

    Pattern(const vector<V_T>& labels) { 
      _data.insert(labels.begin(), labels.end());
      _sup_ok = false;
      _is_frequent = false;
      _is_maximal = false;
      _status_known = false;
    }

    Pattern() {
      _sup_ok = false;    
      _is_frequent = false;
      _is_maximal=false;
      _status_known=false;
    }

   ~Pattern() {
   }

   void add_vertex(V_T v) {
     pair<VIT, bool> ret_type = _data.insert(v);
     assert(ret_type.second == true);
     _sup_ok = false;
   }

   void remove_vertex(V_T v) {
     _data.erase(v);
     _sup_ok = false;
   }

   bool exists(const V_T& vertex) const {
     return (_data.find(vertex) != _data.end()) ?  true : false;
   }

   PAT* clone() const {
#ifdef PRINT
      cout << "In clone:\n";
#endif
      PAT* clone = new PAT();
      clone->_data.insert(_data.begin(), _data.end());
      clone->_vat = _vat; 
      clone->_is_frequent= false;
      clone->_is_maximal = false;
      clone->_status_known=false;
#ifdef PRINT
      cout << "Got the new pattern, returing from clone ..." << endl;
#endif
      return clone;
    }

    size_t size() const {
      return _data.size();
    }

		size_t vat_size() const{
			return _vat.size();	
		}
    void set_vat(const vector<int>& v) {
      _vat = v;
      _sup_ok = true;
    }

    void set_sup_ok() {
      _sup_ok=true;
    }

    bool get_sup_ok() const {
      return _sup_ok;
    }

    bool is_frequent() const {
      assert(_sup_ok == true);
      return _is_frequent;
    }
    
/*
    void set_canonical_code() {
      stringstream ss;
      std::copy(_data.begin(), _data.end(), ostream_iterator<V_T>(ss, "_"));
      _canonical_code = ss.str(); 
    }
*/

    const vector<int>& get_vat() const {
      return _vat;
    }
    
    const set<V_T>& get_data() const {
          return _data;
     }
    void print_vat() const {
      cout <<"\nVAT:Size(" << _vat.size() << ")\n";
      std::copy(_vat.begin(), _vat.end(), ostream_iterator<int>(cout," "));
      cout << endl;
    }

    void join_vat(PAT* p) {
#ifdef PRINT
      cout << "Before join: vat size:" << _vat.size() << endl;
#endif
      vector<int> out_vector;
      set_intersection(_vat.begin(), _vat.end(), p->_vat.begin(), p->_vat.end(),
                back_inserter(out_vector));
      _vat = out_vector;
#ifdef PRINT
      cout << "After join: vat size:" << _vat.size() << endl;
#endif
    }

    void set_freq() {
      _is_frequent = true;
    }

    std::string canonical_code() const { 
      
      stringstream ss;
      std::copy(_data.begin(), _data.end(), ostream_iterator<V_T>(ss, "_"));
      //cout << "canonical code: "<< ss.str()<<endl;
      return ss.str();
    } 
 
    void add_tid_to_vat(int tid) {
      vector<int>::iterator it;
      it = lower_bound(_vat.begin(), _vat.end(), tid);
      _vat.insert(it, tid);
    }

    void add_tid_to_vat(vector<int>::iterator it, int tid){
      _vat.insert(it, tid);
    }

    bool status_known() const {
      return _status_known;
    }

    void set_status_known() {
      _status_known = true;
    }

    bool is_max() const {
      assert(_status_known == true);
      return _is_maximal;
    }

    void set_max() {
      _status_known = true;
      _is_maximal = true;
      _is_frequent = true;
    }
  
    int sub_pat_cnt() const {
      return _data.size();
    }

    CIT_PAIR pat_iterator() const {
      return make_pair(_data.begin(), _data.end());
    }
    
    bool _is_frequent;
    bool _is_maximal;
    

  private:
    vector<int> _vat;
    set<V_T> _data;
    // std::string _canonical_code;
    bool _sup_ok;
    bool _status_known;
    double _belief_score;
}; 


template<typename V_TYPE>
ostream& operator<< (ostream& ostr, const Pattern<V_TYPE>& p){

  std::copy(p._data.begin(), p._data.end(), ostream_iterator<V_TYPE>(cout,"_"));
  cout << "(" << (p._data).size() << ")" << endl;
  return ostr;
}

#endif
