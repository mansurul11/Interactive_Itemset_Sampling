#ifndef _LATTICE_NODE_H
#define _LATTICE_NODE_H

#include <ext/hash_set>
#include <algorithm>
#include "helper_funs.h"
#include "pattern_factory.h"
#include "pattern.h"
//#include "random_walk_manager_freq.h"

template <typename PAT >
struct lattice_node
{

  typedef lattice_node<PAT> L_NODE;
  typedef typename PAT::VERTEX_T V_T;

  string get_key() {
    
    std::string min_dfs_cc = _pat->canonical_code();
    return min_dfs_cc;
  }
 
  lattice_node(PAT* p) {
    _pat = p;
    _is_frequent = false;
    _is_maximal = false;
    //mine vector<int>_get_tidset;
    set<V_T> myset = _pat->get_data();
    //mine _get_tidset = _pat->get_vat();
    unsigned int i=1;
    //cout<< "size "<<_get_tidset.size();
    typename set<V_T>::iterator it;
    /*for (it=myset.begin(); it!=myset.end(); it++)
        cout << " " << *it;*/

    /*for(i=0;i<_get_tidset.size();i++)
    {
    	//cout << "tid :"<<_get_tidset.at(i);
    	_belief_score += score.at(_get_tidset.at(i));

    }*/
    //cout<<"end and score"<<_belief_score << endl;
  }

  ~lattice_node() {
    delete _pat;
  }
  
  bool _is_maximal;
  bool _is_frequent;
  bool _is_processed; // it is true, when we know all neighbors and their status of this pattern
  int _its_degree;
  PAT* _pat;
  vector<L_NODE*> _neighbors;
  vector<int> _neighbor_status;  // -1 for infrequent, 1 for frequent, #degree for maximal
  double _belief_score;
};

#endif
