#ifndef _PATTERN_FACTORY_
#define _PATTERN_FACTORY_

#include "pattern.h"
#include "database.h"
#include "random.h"


template <typename PAT>
class PatternFactory {

  public:

    typedef typename PAT::VERTEX_T V_T;
    typedef typename PAT::VIT VIT;
    typedef Database<PAT> DATABASE;
    typedef typename PAT::CIT_PAIR CIT_PAIR;

    static PatternFactory<PAT>* instance(DATABASE* d) {
      if (_instance == 0) {
        _instance = new PatternFactory<PAT>(d);
      }
      return _instance;
    }  

    PAT* make_size_one_pattern(V_T src) const {
      PAT* p = new PAT();
      p->add_vertex(src);
      const vector<int>& vat = _d->get_vertex_vat(src); 
      p->set_vat(vat);
      p->set_sup_ok();
      int minsup = _d->get_minsup();
      if (vat.size() > minsup)
        p->set_freq();
      return p;
    }

    PAT* get_one_frequent_pattern() {
      int minsup = _d->get_minsup();
      V_T one_v = _d->get_a_random_freq_vertex();
      PAT* cand_pat = make_size_one_pattern(one_v);
   return cand_pat;
    }

    PAT* get_one_maximal_pattern() {
      int minsup = _d->get_minsup();
      V_T one_v = _d->get_a_freq_vertex();
      PAT* cand_pat = make_size_one_pattern(one_v);
      return extend_until_max(cand_pat, minsup);
    }

    // Check a pattern's staus, is it freq, or maximal compute its VAT also, 
    // for infrequent VAT is INCOMPLETE
    void get_status_optimal(PAT*& pat, bool& is_freq, bool& is_max) {
      int minsup = _d->get_minsup();
      assert(pat->get_sup_ok() == true);
      if (pat->status_known() == true) {
        is_freq=pat->_is_frequent;
        is_max=pat->_is_maximal;
        return;
      }

      PAT* cand_pat=0;
      vector<V_T> ext_list;
      _d->get_possible_ext(pat, ext_list); 
      bool extended = false; 
      for (int i=0; i<ext_list.size(); i++)  {
        V_T v = ext_list[i];
        const vector<int>& its_vat = _d->get_vertex_vat(v);
        cand_pat= pat->clone();
        cand_pat->add_vertex(v);
        vector<int> other_vat = pat->get_vat();
        vector<int> out_list;
        _d->vat_join(other_vat, its_vat, out_list);
        if (out_list.size() >= minsup) {
          is_freq = true;
          is_max = false;
          pat->set_status_known();
          delete cand_pat;
          return;
        }
        else {
          delete cand_pat;
        }
      }
      pat->set_max();
      is_freq = true;
      is_max= true;
    }

    

    PAT* get_random_maximal() const {
      int minsup = _d->get_minsup();
      V_T one_v = _d->get_a_random_freq_vertex();
      PAT* pat = make_size_one_pattern(one_v);
      assert(pat->get_sup_ok() == true);

      PAT* cand_pat=0;
      vector<V_T> ext_list;
      _d->get_possible_ext(pat, ext_list); 
      bool extended = false; 
      while (true) {
        if (ext_list.size() == 0) {
          pat->set_max();
          return pat;
        }
        int i = boost_get_a_random_number(0, ext_list.size());
        V_T v = ext_list[i];
        const vector<int>& its_vat = _d->get_vertex_vat(v);
        cand_pat= pat->clone();
        cand_pat->add_vertex(v);
        vector<int> other_vat = pat->get_vat();
        vector<int> out_list;
        _d->vat_join(other_vat, its_vat, out_list);
        if (out_list.size() >= minsup) {
          delete pat;
          pat = cand_pat;
        }
        else {
          delete cand_pat;
        }
        ext_list.erase(ext_list.begin()+i);
      }
    } 

    // precondition: given pattern is entirely consistent with correct support count
    // and support list
    PAT* pattern_with_vertex_removed(PAT* p, V_T v) {
#ifdef PRINT
      const vector<int>& cur_vat = p->get_vat();
      cout << "its vat size:" << cur_vat.size() << endl;
      std::copy(cur_vat.begin(), cur_vat.end(), ostream_iterator<int>(cout, " "));
      cout << endl;
#endif
      PAT* clone = p->clone();
      clone->remove_vertex(v); 

      // now support list needs to be updated
      _d->get_sup_directly(clone);
#ifdef PRINT
      cout << "done\n";
#endif
      return clone;
    }

    void get_sub_patterns(PAT* pat, vector<PAT*>& sub_patterns) {
      if (pat->size() == 0) return;
      CIT_PAIR it_pair = pat->pat_iterator();
      for (;it_pair.first != it_pair.second; it_pair.first++) {
        PAT* sub_pat = pattern_with_vertex_removed(pat, *(it_pair.first));
        sub_patterns.push_back(sub_pat);
      }
    }
 int get_super_degree(PAT*& pat) {
#ifdef PRINT
      cout << "In get_super_degree" << endl;
#endif
      if (pat->size()==0) return get_database()->level_one_freq_size();
      int minsup = _d->get_minsup();
      assert(pat->get_sup_ok() == true);
      int ret_val=0;
      PAT* cand_pat=0;
      vector<V_T> ext_list;
      _d->get_possible_ext(pat, ext_list); 
      bool extended = false; 
      for (int i=0; i<ext_list.size(); i++)  {
        V_T v = ext_list[i];
        const vector<int>& its_vat = _d->get_vertex_vat(v);
        cand_pat= pat->clone();
        cand_pat->add_vertex(v);
        vector<int> other_vat = pat->get_vat();
        vector<int> out_list;
        _d->vat_join(other_vat, its_vat, out_list);
        if (out_list.size() >= minsup) {
          ret_val++;
        }
        delete cand_pat;
      }
#ifdef PRINT
      cout << "returning ret_val " << ret_val<< endl; 
#endif
      return ret_val;
    }
    void get_freq_super_patterns(const PAT* pat, vector<PAT*>& super_patterns) {
#ifdef PRINT
      cout<<"In call to get_all_frequent_super_pattern\n";
#endif
    
      int minsup = _d->get_minsup();
      assert(pat->get_sup_ok() == true);
      PAT* cand_pat=0;
      vector<V_T> ext_list;
      _d->get_possible_ext(pat, ext_list); 
      const vector<int>& its_vat = pat->get_vat();

      for (int i=0; i<ext_list.size(); i++)  {
        V_T v = ext_list[i];
        const vector<int>& other_vat = _d->get_vertex_vat(v);
        cand_pat= pat->clone();
        cand_pat->add_vertex(v);
        vector<int> out_list;
        _d->vat_join(other_vat, its_vat, out_list);
        if (out_list.size() < minsup) {
          delete cand_pat;
        }
        else {
            cand_pat->set_vat(out_list);
            cand_pat->set_sup_ok(); 
            cand_pat->set_freq(); 
#ifdef PRINT    
            cout << *cand_pat << endl;
#endif
            super_patterns.push_back(cand_pat);
        }
      }
    }


    // extend an SINGLE vertex pattern until it is maximal
    PAT* extend_until_max(PAT*& pat, const int& minsup) {
      
#ifdef PRINT
      cout<<"In call to extend_until_max\n";
#endif 
      assert(pat->get_sup_ok() == true);
      PAT* cand_pat=0;
      vector<V_T> ext_list;
      _d->get_possible_ext(pat, ext_list); 
#ifdef PRINT
      cout << "Possible extention size:" << ext_list.size() << endl;
#endif
      vector<int> its_vat = pat->get_vat();
      vector<V_T> to_be_added;

      for (int i=0; i<ext_list.size(); i++)  {
        V_T v = ext_list[i];
        const vector<int>& other_vat = _d->get_vertex_vat(v);
        vector<int> out_list;
        _d->vat_join(other_vat, its_vat, out_list);
        if (out_list.size() >=  minsup) {
          to_be_added.push_back(v);
          its_vat = out_list; 
          out_list.clear();
        }
      }
      for (int i=0; i<to_be_added.size(); i++) {
        pat->add_vertex(to_be_added[i]);  
      }
      pat->set_vat(its_vat);
#ifdef PRINT
      cout << "exiting with the following max sub-graph:\n";
      cout << *pat << endl;
#endif
      pat->set_max();
      return pat;
    }

    static PatternFactory<PAT>* set_static_data() {
      return 0;
    }

    DATABASE* get_database() const {
      return _d;
    }
//NEW CODE START
    int get_class_pf(int tid)
    {
        return _d->get_class_d(tid);
    }
    int get_vertex_support_pf(V_T v)
    {
        return _d->get_vertex_support(v);
    }
    double get_itemscore_pf(V_T item)
    {
        return _d->get_itemscore_d(item);
    }
    void update_itemscore_pf(V_T pattern,double increment)
    {
        _d->update_itemscore_d(pattern,increment);
    }
    bool find_intersting_pf(set<V_T> pattern)
    {
        return _d->find_intersting(pattern);
    }
    double find_NewQscore_pf(set<V_T> pattern)
    {
        return _d->find_NewQscore(pattern);
    }
    void get_final_sample_distribution_pf()
    {
        _d->get_final_sample_distribution_d();
    }
    int level_one_freq_size_pf()
    {
        return _d->level_one_freq_size();
    }
    int getfileindex_pf()
    {
        return _d->getfileindex_d();
    }

  protected:
    PatternFactory(DATABASE* d) {
      _d = d;
    }
   
  private:

  DATABASE* _d;
  static PatternFactory<PAT>* _instance;
};

#endif
