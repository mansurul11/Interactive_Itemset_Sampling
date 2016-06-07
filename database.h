
#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <fstream>
#include <exception>
#include <iostream>
#include <iterator>
#include <functional>
#include <ctime>
#include <map>
#include "pattern.h"
#include "element_parser.h"
#include "StringTokenizer.h"
#include "random.h"

using namespace std;

class MinSupException: public exception
{
  virtual const char* what() const throw() {
    cout << "MinSupException: minimum suppport is greater than database size; No pattern is frequent\n";
  }
} min_sup_ex;

class FileOpenErrorException: public exception
{
  virtual const char* what() const throw() {
    cout << "FileOpenErrorException: input file could not be opened\n";
  }
} file_open_ex;

class FileFormatException: public exception
{
  virtual const char* what() const throw() {
    cout << "FileFormatException: input file does not has the desired format\n";
  }
} file_format_ex;
// random generator function:
ptrdiff_t myrandom (ptrdiff_t i) { srand (time(NULL));return rand()%i;}

// pointer object to it:
ptrdiff_t (*p_myrandom)(ptrdiff_t) = myrandom;
// class to sort an array to get sorted-index
template<class T, bool ascending> struct index_cmp {
index_cmp(const T arr) : arr(arr) {}
  bool operator()(const size_t a, const size_t b) const {
    if (ascending)
      return arr[a] < arr[b];
    else
      return arr[b] < arr[a];
  }
  const T arr;
};

template <typename PAT_T>
class Database {
  public:
    typedef typename PAT_T::VERTEX_T V_T;
    typedef typename PAT_T::CIT_PAIR CIT_PAIR;
    typedef map<V_T, vector<int> > MAP_T;
    typedef typename MAP_T::const_iterator MAP_CIT;
    typedef typename MAP_T::iterator MAP_IT;
    typedef  map<V_T,double> MAP_I;
    typedef typename MAP_I::iterator MAP_IIT;
    typedef typename PAT_T::VIT VIT; 
    typedef map<int, V_T> MAP_SUPPORT;
    typedef typename MAP_SUPPORT::iterator MAP_SUPPORT_IT;
    Database(const char* filename) {
        out.open("output/final_sample_distribution");
        ifstream infile(filename, ios::in);
        if (!infile) {
            throw file_open_ex;
        }
        ifstream readitem(filename, ios::in);
        if (!readitem) {
            throw file_open_ex;
        }
        int pos = 0;
        int graph_no = -1;
        fileindex=0;
        read_next(infile);
        // read_items(readitem);
        cout << "total itemset in database:" << _graph_store.size() << endl;
    }

    V_T get_a_freq_vertex() const {
        MAP_CIT cvit = _vertex_vat.begin();
        return cvit->first; 
    }

    V_T get_a_random_freq_vertex() const {
        int idx = boost_get_a_random_number(1, _vertex_vat.size());
        MAP_CIT cvit = _vertex_vat.begin();
        while (idx) {cvit++; idx--;}
        return cvit->first; 
    }

    int level_one_freq_size() const {return _vertex_vat.size();}

    const vector<int>& get_vertex_vat(V_T v) const {
      MAP_CIT cvit = _vertex_vat.find(v);
      if (cvit != _vertex_vat.end()) {
        return cvit->second;
      }
      return Database<PAT_T>::_no_data;
    }

    void remove_infrequent_vertices() {
      vector<V_T> singleitem;
      ofstream print("interesting_items");
      MAP_SUPPORT sort_by_support;
      MAP_SUPPORT_IT sit;
      MAP_IT vit = _vertex_vat.begin();
      for (; vit != _vertex_vat.end();) {
        if (vit->second.size() < _minsup) {
          _vertex_vat.erase(vit++);
        }
        else {
   sort_by_support[vit->second.size()] = vit->first;
   singleitem.push_back(vit->first);
          ++vit;
        }
      }
      random_shuffle(singleitem.begin(), singleitem.end(),p_myrandom);
 
      int selectitem = singleitem.size()*0.3;
      cout << "Total level-one frequent"<< sort_by_support.size() << "\t"<<user_fav.size()<< endl;
      int i = 0;
      for(int i=0; i< selectitem ;i++)
      {  
          user_fav.push_back(singleitem[i]);
          print << singleitem[i] << "\n";
      }
      cout << endl;
    }
    void read_items(ifstream& readitem)
    {
        std::string one_line;
        int pos = 0; 
        while (1) {
            std::getline(readitem, one_line);
            if (one_line.length() < 1) break;
            StringTokenizer strtok = StringTokenizer(one_line, " ");
            int numToks = strtok.countTokens();
            if (numToks < 3) {
                throw file_format_ex;
            }
            int tid = strtok.nextIntToken();
 
            strtok.nextIntToken();  // skipping the time stamp
            int num_items = strtok.nextIntToken();
            strtok.nextIntToken(); //get class information.
            for (int i=0; i<num_items; i++) {
                int item = strtok.nextIntToken();
                MAP_IIT item_it;
                if((item_it = item_map.find(item)) != item_map.end())
                {  
                } 
                else
                {  
                    item_map.insert(make_pair(item,1));
                }
            }
        }
        MAP_IIT item_it;
    }
    int read_next(ifstream& infile){
        std::string one_line;
        Pattern<V_T>* pat = 0;
        int pos = 0; 
        while (1) {
            std::getline(infile, one_line);
            if (one_line.length() < 1) break;
            StringTokenizer strtok = StringTokenizer(one_line, " ");
            int numToks = strtok.countTokens();
            if (numToks < 3) {
                throw file_format_ex;
            }
            int tid = strtok.nextIntToken();
 
            strtok.nextIntToken();  // skipping the time stamp
            int num_items = strtok.nextIntToken();
            int _which_class = strtok.nextIntToken(); //get class information.
            _class_data.push_back(_which_class);
            PAT_T* pat = new PAT_T();
            for (int i=0; i<num_items; i++) {
                V_T v_lbl = _vl_prsr.parse_element(strtok.nextToken().c_str());
                MAP_IT vat_it;
                if ((vat_it = _vertex_vat.find(v_lbl)) != _vertex_vat.end()) {
                    vat_it->second.push_back(pos);
                }
                else {
                    vector<int> vat;
                    vat.push_back(pos);
                    _vertex_vat.insert(make_pair(v_lbl, vat));
                }
                pat->add_vertex(v_lbl);
            }
           _graph_store.push_back(pat);
           pos++;
      }
    
    }

    void print_database() const {
      for (int i=0; i<_graph_store.size(); i++) {
        cout << *(_graph_store[i]);
      }
    }

    // In this case, the current VAT is a VAT of a super-pattern
    bool get_sup_directly(PAT_T* pat) {
#ifdef PRINT_BACKEND
      cout << "In get_sup_directly" << endl;
      cout << "getting support for this pattern:" << endl;
      cout << *pat << endl;
#endif
      if (pat->size() == 0) {
        vector<int> sup_list(_graph_store.size());
        for (int i=0; i < _graph_store.size(); i++) 
          sup_list[i]=i; 
        
        pat->set_vat(sup_list);
        assert(sup_list.size() >= _minsup);
        pat->set_freq(); 
        return true;
      }
      CIT_PAIR it_pair = pat->pat_iterator();
      
      vector<int> sup_list = get_vertex_vat(*it_pair.first);
      it_pair.first++;
      vector<int> out_list;
      
      for (;it_pair.first != it_pair.second; it_pair.first++) {
        const vector<int>& its_vat = get_vertex_vat(*it_pair.first);
        vat_join(its_vat, sup_list, out_list);
        sup_list = out_list; 
        out_list.clear();
      }
      pat->set_vat(sup_list);
      assert(sup_list.size() >= _minsup);
      pat->set_freq(); 
    }
 

    void get_possible_ext(const PAT_T* p, vector<V_T>& ret_val) {
      MAP_CIT cmit;
      for (cmit = _vertex_vat.begin(); cmit != _vertex_vat.end(); cmit++) {
        V_T vertex = cmit->first;
        if (!p->exists(vertex)) {
          ret_val.push_back(vertex); 
        } 
      }
    }

    static void vat_join(const vector<int>& v1, const vector<int>& v2, vector<int>& out_list) {
      int i=0,j=0;
      while (i<v1.size() && j<v2.size()) {
        if (v1[i] < v2[j]) {
          i++; 
        }
        else if(v2[j] < v1[i]) { 
          j++;
        }
        else {
          out_list.push_back(v1[i]);
          i++;j++;
        }
      }
    }
    static vector<int> set_static_data() {
      vector<int> temp;
      return temp;
    }

    int get_minsup() const {return _minsup;}
    void set_minsup(int minsup) {
      if (minsup > _graph_store.size()) {
        throw min_sup_ex;
      }
      _minsup =minsup;
    }

    int size() const { return _graph_store.size();}

  void initialize_item_score()
  {
      typename vector<V_T>::iterator it;
      MAP_IT vit = _vertex_vat.begin();
      for (; vit != _vertex_vat.end();vit++) {
          item_map.insert(make_pair(vit->first,1));
      }
      MAP_IIT item_it = item_map.begin();
      
  }
  void get_single_item_summary()
  {
      int i,j;
      double pos_cls,neg_cls;
      vector<int> _tidset;
      cout<<"insummary"<<endl;
      ofstream out2("output/single_edge_summary");
      MAP_IT vit = _vertex_vat.begin();
      for (; vit != _vertex_vat.end();vit++) {
        
          _tidset = vit->second;
          for(j=0;j<_tidset.size();j++)
          {
              if(_class_data[_tidset[j]]==1)
              {
                  pos_cls+=1;
              }
              else
              {
                  neg_cls+=1;     
              }
           }
           out2<<vit->first<<"\t"<<pos_cls/(pos_cls+neg_cls)<<"\t"<<pos_cls<<"\t"<<neg_cls<<"\t"<<pos_cls+neg_cls<<endl;
           _tidset.clear();
           pos_cls=0;
           neg_cls=0;
   
       }
   
    }
    int get_class_d(int tid)
    {
        return _class_data[tid];
    }
    double get_itemscore_d(V_T item)
    {
        MAP_IIT item_it;
        if((item_it = item_map.find(item)) != item_map.end())
        {  
            //cout <<"item:"<<item<<"score="<< item_it->second<<endl;  
            return item_it->second; 
        } 
        else
        {  
            cout << "no item found" <<endl;
        }
    }
    double find_NewQscore(set<V_T> pattern)
    {
         typename vector<V_T>::iterator it;
         VIT vit;
         double check=0;
         double size = pattern.size();
         for(vit = pattern.begin(); vit!= pattern.end(); vit++)
         {
             if((it = find(user_fav.begin(), user_fav.end(), *vit)) != user_fav.end())
             {
                 check++;
             }
             it = user_fav.begin();
          }
          if((check/size)*100 > 51)
              return 1;
          else return 0;
     }
     bool find_intersting(set<V_T> pattern)
     {
         typename vector<V_T>::iterator it;
         VIT vit;
         double check=0;
         double size = pattern.size();
         for(vit = pattern.begin(); vit!= pattern.end(); vit++)
         {
             if((it = find(user_fav.begin(), user_fav.end(), *vit)) != user_fav.end())
             {
                 check++;
             }
             it = user_fav.begin();
         }
         if((check/size)*100 > 51)
             return true;
         else 
             return false;   
     }
     void update_itemscore_d(V_T pattern,double increment)
     {
         VIT vit;
         MAP_IIT item_it;
         if((item_it = item_map.find(pattern)) != item_map.end())
         {  
             item_it->second *=  increment; 
         }  
         else
         {   
             cout << "no item found" <<endl;
         } 
 
      }
      void get_final_sample_distribution_d()
      {
          MAP_IIT item_it;
          fileindex++;
          char filename[50]; 
          ofstream TP;
          int n=sprintf(filename, "%d",fileindex); 
          TP.open(filename);
          for(item_it = item_map.begin(); item_it!= item_map.end(); item_it++)
          { 
              cout << item_it->first << " "<< item_it->second<<" ";
          }
          TP.close();
      }
      int getfileindex_d()
      {
          return fileindex;
      }
      int get_vertex_support(V_T v)
      {
          return _vertex_vat[v].size();
      }
  private:
    map<V_T, vector<int> > _vertex_vat;
    vector<PAT_T*> _graph_store;  // store all the graph patterns
    element_parser<V_T> _vl_prsr;
    static vector<int> _no_data;       // dummy vector to return reference to null data
    int _minsup;
    vector<int>_class_data;// vector for storing class information 
    MAP_I item_map; 
    ofstream out;
    int fileindex;
    vector<V_T> user_fav;
    
};

#endif
