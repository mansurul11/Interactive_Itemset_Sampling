#ifndef _RANDOM_WALK_MANAGER2_H_
#define _RANDOM_WALK_MANAGER2_H_

#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <set>
#include <ext/hash_set>
#include <ext/hash_map>
#include "helper_funs.h"
#include "pattern_factory.h"
#include "lattice_node.h"
#include "random.h"
#include <sstream>
#include "pattern.h"
#include <math.h>
#include <vector>
#include <map>
#include <string>
/**
 * This version sample frequent itemset patterns from user's interactive feedback
 */
template<class PAT>
class RandomWalkManager_Freq
{

  public:

  typedef lattice_node<PAT> LATTICE_NODE;
  typedef HASHNS::hash_map<string, LATTICE_NODE*, hash_func<string>, equal_to<string> > NODE_MAP;
  typedef typename NODE_MAP::iterator NS_IT;
  typedef typename NODE_MAP::const_iterator CONST_NS_IT;

  typedef HASHNS::hash_map<string, int, hash_func<string>, equal_to<string> > FREQ_CNT_MAP;
  
  typedef typename FREQ_CNT_MAP::iterator FC_IT;
  typedef Database<PAT> DATABASE;
  typedef PatternFactory<PAT> PATTERN_FACTORY;
  typedef typename PAT::VERTEX_T V_T;
  typedef typename PAT::VIT VIT;
  typedef typename vector<V_T>::iterator vector_it;
  
  RandomWalkManager_Freq(DATABASE* d, int iter,int feedb) {
    _last = 0;
    _pf = PATTERN_FACTORY::instance(d);
    _maxiter = iter;
    int count;
    addrandomedge = 0;
    startrandomedgeflag = 1;
    numberoflatticenode = 0;
    previouslyreportsize=0;
    numberof_feedback=0;
    feed = feedb;
    alpha.open("acceptance_Prob");
  }

  PatternFactory<PAT>* get_pat_factory() const {
    return _pf;
  }

  // random walk manager initialize with a maximal pattern node
  LATTICE_NODE* initialize() {
    PAT* p =_pf->get_one_frequent_pattern();
   // cout <<"canonical code:" << p->canonical_code();
    //exit(1);
    if (p->_is_frequent) {
      cout << "Yes, it is frequent\n";
    }
    else {
      cout << "ERROR:this pattern is infrquent\n";
      exit(1);
    }
    LATTICE_NODE* ln = create_lattice_node(p);
    process_node(ln);
    return ln;
  }

  void build_lattice(LATTICE_NODE* current, int &iter)
  {
      LATTICE_NODE* next;
      PAT* p;
      while (true) {
          process_node(current);
          p = current->_pat;
          iter++;
          if (iter >= _maxiter) {
              return;   
          }
          next = get_next(current);
          cout<<"iter:"<<iter<<endl;
          _last = p;
          current = next;
      }
   
   }

  void walk(LATTICE_NODE* current, int &iter) {

     vector<double> box;
     ofstream out_("all_pattern"); //file for printing all sampled pattern 
     ofstream out2("unique_pattern"); // only unique sampled pattern
     ofstream out1("output/unique_visit_count"); // For debug
     ofstream out5("output/iterOFfeedback"); // For Debug
     int filename=1;
     NS_IT nmit;
     LATTICE_NODE* next;
     PAT* p;
     double score;
     int count=0;
     int previter=0;
     while (true) {
         process_node(current);
         p = current->_pat;

         iter++;
         if(iter % 500 == 0 && startrandomedgeflag == 1) // add random edge in the pattern lattice
         {
             cout << "inside first insert_random_edge <<" << iter << " " << startrandomedgeflag <<endl;
             //print_lattice();
   
             insert_random_edge();
             //print_transitional_matrix(iter);
             
             addrandomedge = 1;
             startrandomedgeflag = 0;
   
         }
         if(addrandomedge == 1)
         {
             count=0;
             nmit = _node_map.begin();
             while(nmit != _node_map.end())
             {
                 if (nmit->second->_is_processed){
                     count++;
                 }
                 nmit++;
             } 
             previter=count;
             if(count/numberoflatticenode >= 3)
             {
                 cout << "inside double node check << " << count << " "<< numberoflatticenode <<endl;
                 insert_random_edge(); 
                 //print_transitional_matrix(iter); 
             }
          }
        
          // End of execution
          if (iter >= _maxiter) {
              cout << "Dist:"<<_maxiter<<"totalPat:"<<_freq_cnt.size()<<endl;
              FC_IT mit;
              double avg_Qscore=0;
              //cout<<"pattern visited:"<<endl;
              int i=0;
              for(mit = _freq_cnt.begin(); mit != _freq_cnt.end();mit++,i++)
              {
                  out1 << mit->first<<"\t"<<mit->second<<endl;
              }
              for(i = 0; i< reward_vector.size();i++)
              {
                  cout << reward_vector[i]<< ",";
              }
              cout << endl; 
              _pf->get_final_sample_distribution_pf(); //print final sample distribution
  
              return;
           }

           if (iter >=1) {
               string cc = p->canonical_code();
               double _conf_pat = calculate_Qscore(p);
               double current_score = calculate_MHscore(current);
               out_<< iter<<"\t"<<_conf_pat<< "\t"<<cc<<"\t"<<p->vat_size()<<"\t"<<current_score<<endl; 
               FC_IT mit = _freq_cnt.find(cc);
               if (mit != _freq_cnt.end()) {
                   mit->second++;
               }
               else 
               {
                  _freq_cnt.insert(make_pair(cc, 1));
                  double _conf_pat = calculate_Qscore(p);
                  out2<<cc<<"\t"<<p->size()<<"\t"<<_conf_pat<<"\t"<<p->vat_size()<<endl;      
               }

           }
    
           if (iter % 50 == 0 && numberof_feedback <=feed) {
               string cc = p->canonical_code();
               double _conf_pat = calculate_Qscore(p);
               FC_IT fit = _feedback_cnt.find(cc);
               if (fit != _feedback_cnt.end()) {
            
               }
               else 
               {
                   _feedback_cnt.insert(make_pair(cc, 1));
                   //out5<<cc<<"\t"<<p->size()<<"\t"<<_conf_pat<<endl;  
                   if(get_feedback(current))
                       out5<<cc<<"\t"<< iter <<"\t"<<_conf_pat<< endl;          
               }

           }
      
      
           next = get_next(current);
  
           cout<<"rela iter:"<<iter<< "feed " << numberof_feedback <<"\t" <<previter << "\t"<<numberoflatticenode<<endl;
           _last = p;
           current = next;
      }
   
  }


//NEW MH
  LATTICE_NODE* get_next(LATTICE_NODE* current) {
     int total = current->_neighbors.size();
     int neighbors_count_cur,neighbors_count;
     double accept_prob;
     unsigned int get_position;
     double current_score=0;
     double neighbors_score=0;
     double current_score_log,neighbors_score_log;
     double get_uni;
     int cls;
     double sum;
     int reject=0;
     string cc_cur = current->_pat->canonical_code();
     current_score = calculate_MHscore(current);
     double qscore = calculate_Qscore(current->_pat);
     int size = current->_pat->size();
     //current_score = current_score/(size+0.1);
     while(1)
     {
         reject++;
         get_position = boost_get_a_random_number(0, total);
         neighbors_count = current->_neighbor_status[get_position];
         string cc_n = current->_neighbors[get_position]->_pat->canonical_code();
         neighbors_score = calculate_MHscore(current->_neighbors[get_position]);
         size = ((current->_neighbors[get_position])->_pat)->size();
         //neighbors_score = neighbors_score/(size+0.1);
         accept_prob = ((neighbors_score*total)/(current_score*neighbors_count) >= 1)? 1:
             (neighbors_score*total)/(current_score*neighbors_count);

         get_uni = random_uni01();
         if(get_uni <= accept_prob)
         {
             double q2score = calculate_Qscore(current->_neighbors[get_position]->_pat);
             double x =neighbors_score/current_score ;
             alpha <<cc_cur<<"\t"<<qscore<<"\t"<<current_score<<"\t"<<cc_n<<"\t"<<q2score<<"\t"<<neighbors_score<<"\t"<< x <<"\t"<<accept_prob<< endl;
             return current->_neighbors[get_position];
         }
     }

  }

  LATTICE_NODE* create_lattice_node(PAT*& p) {
    //p->set_canonical_code();
    std::string min_dfs_cc = p->canonical_code();
    LATTICE_NODE* node = exists(min_dfs_cc);
    if (node == 0) {  // new pattern
      node = new LATTICE_NODE(p);
      node->_is_processed = false;
      insert_lattice_node(min_dfs_cc, node);
    }
    else {
      delete p;
      p = node->_pat;
    }
    return node;
  }

  LATTICE_NODE* exists(string p) {;
    CONST_NS_IT it = _node_map.find(p);
    return (it != _node_map.end())? it->second : 0;
  }

  void insert_lattice_node(string p, LATTICE_NODE* ln) {
    _node_map.insert(make_pair(p, ln));
 //cout << "no of pattern discover = " << _node_map.size() <<endl;
  }

  void process_node(LATTICE_NODE* n) {
    if (n->_is_processed) return;
    PAT* p = n->_pat;
    assert(p->get_sup_ok());
    //cout << "Current pattern:\n";
    //cout << *p;
    vector<PAT*> neighbors;
    if (p->is_frequent()) {
      //cout << "gettin frequent super_patterns" << endl;
      _pf->get_freq_super_patterns(p, neighbors); 
      //cout << "done, gettin frequent sub patterns" << endl;
      _pf->get_sub_patterns(p, neighbors); 
      //cout << "done" << endl;
    }
    else {
      cout << "ERROR: In this walk we are traversing only frequent patterns (1)\n";
      exit(1);
    }

    //cout << "Its neighbors: " <<neighbors.size()<< endl;

    for (int i=0; i<neighbors.size(); i++) {
      PAT* one_neighbor = neighbors[i];

      //cout << "one:"<<*one_neighbor<<endl;

      LATTICE_NODE* ln = create_lattice_node(one_neighbor);
      int status;
      // The first part of the rhs is super-degree-count, and the second part is sub-degree-count
      int its_degree = _pf->get_super_degree(one_neighbor) + one_neighbor->size();
      // cout<<"degree of "<< *one_neighbor<< "is: "<<its_degree<<endl;
      n->_neighbors.push_back(ln);
      // (its_degree > neighbors.size())? n->_neighbor_status.push_back(its_degree): n->_neighbor_status.push_back(neighbors.size());
      //new code
      n->_neighbor_status.push_back(its_degree);
   
    }  
    n->_is_processed=true;
  }

 void insert_random_edge()
 {
     int v1, v2;
     typename std::vector<LATTICE_NODE*>::iterator it;
     NS_IT nmit = _node_map.begin();
     int edges=0; 
     int process_node=0;
     map<int,LATTICE_NODE*> lnode_map;
     int index=0;
     while(nmit != _node_map.end())
     {
         lnode_map.insert(make_pair(index,nmit->second));
         edges += nmit->second->_neighbors.size();
         index++;
 
         nmit++;

     }
     int vertices = lnode_map.size();
     cout << "no of nodes = "<<index<<"previouslyreportsize = "<<previouslyreportsize<<endl;
     //int no_of_level1pat = _pf->level_one_freq_size_pf();
     edges = edges/2;
     //cout << "no of edges = "<<edges <<" " <<edges<<endl;
     int no_of_randomedge = edges*0.2;
     while(no_of_randomedge>0)
     { 
         v1 = boost_get_a_random_number(previouslyreportsize, vertices);
         v2 = boost_get_a_random_number(0, vertices);
   
         //cout << "edges = " <<no_of_randomedge << " p1=" << lnode_map[v1]->_pat->canonical_code() << " p2= " << lnode_map[v2]->_pat->canonical_code() << endl;
         if(v1 == v2) continue;
         it = find(lnode_map[v1]->_neighbors.begin(),lnode_map[v1]->_neighbors.end(),lnode_map[v2]); 
         if(it != lnode_map[v1]->_neighbors.end()) continue;
         //cout << "edges = " <<no_of_randomedge << " p1=" << lnode_map[v1]->_pat->canonical_code() << " p2= " << lnode_map[v2]->_pat->canonical_code() << endl;
         string min_dfs_cc = lnode_map[v1]->_pat->canonical_code();
         nmit = _node_map.begin();   
         nmit = _node_map.find(min_dfs_cc);
         nmit->second->_neighbors.push_back(lnode_map[v2]);
         nmit->second->_neighbor_status.push_back(lnode_map[v2]->_neighbors.size()+1);

         it = find(lnode_map[v2]->_neighbors.begin(),lnode_map[v2]->_neighbors.end(),lnode_map[v1]); 
         if(it != lnode_map[v2]->_neighbors.end()) continue;
         min_dfs_cc = lnode_map[v2]->_pat->canonical_code();
         nmit = _node_map.begin();   
         nmit = _node_map.find(min_dfs_cc);
         nmit->second->_neighbors.push_back(lnode_map[v1]);
         nmit->second->_neighbor_status.push_back(lnode_map[v1]->_neighbors.size());
         //cout << "add edge between "<< lnode_map[v1]->_pat->canonical_code() <<"and "<< lnode_map[v2]->_pat->canonical_code() <<endl;
         no_of_randomedge--; 
         //cout << "addrandomedge = " <<addrandomedge<<endl;
      }

      vertices = lnode_map.size();
      edges=0;
      //cout << "no of nodes = "<<vertices<<endl;
      nmit = _node_map.begin();
      while(nmit != _node_map.end())
      {  
          if (nmit->second->_is_processed){
              process_node++;
          }
          edges += nmit->second->_neighbors.size();
          nmit++;
      }
      //no_of_level1pat = _pf->level_one_freq_size_pf();
      edges = (edges)/2 ;
      numberoflatticenode =process_node;
      previouslyreportsize = process_node;
      cout << "insert_random_edge finish edges= <<" << edges << " and " << previouslyreportsize <<endl;
 }
 
 bool get_feedback(LATTICE_NODE* feedback_pat)
 {
     int inc=1;
     set<V_T> _get_pattern;
     _get_pattern = (feedback_pat->_pat)->get_data();
     string cc = (feedback_pat->_pat)->canonical_code();
     alpha << cc <<endl;
     if(_pf->find_intersting_pf(_get_pattern))
     {
         numberof_feedback++;
         update_reward_vector_add(_get_pattern);
         update_penalty_vector_delete(_get_pattern);
         update_item_score(feedback_pat,inc);
         cout << "positive feedback\n";
         _pf->get_final_sample_distribution_pf();
        return true;
      }
      else
      {
          numberof_feedback++;
          // cout << "negative feedback\n";
   
          // _pf->get_final_sample_distribution_pf();
          cout << "negative feedback\n";
          update_reward_vector_delete(_get_pattern);
          update_penalty_vector_add(_get_pattern);
          update_item_score(feedback_pat,inc);
          return false;
       }
   
   }

 void update_penalty_vector_add(set<V_T> _get_pattern)
 {
     vector_it it;
     VIT vit;
     for(vit = _get_pattern.begin(); vit!= _get_pattern.end();vit++)
     {
         it = find(penalty_vector.begin(), penalty_vector.end(), *vit);
         if(it==penalty_vector.end())
         {
             penalty_vector.push_back(*vit);
         } 
     } 
 }

 void update_penalty_vector_delete(set<V_T> _get_pattern)
 {
     vector_it it;
     VIT vit;
     for(vit = _get_pattern.begin(); vit!= _get_pattern.end();vit++)
     {
         it = find(penalty_vector.begin(), penalty_vector.end(), *vit);
         if(it!=penalty_vector.end())
         {
             penalty_vector.erase(it);
         } 
     } 
 }

 void update_reward_vector_add(set<V_T> _get_pattern)
 {
     vector_it it;
     VIT vit;
     for(vit = _get_pattern.begin(); vit!= _get_pattern.end();vit++)
     {
         it = find(reward_vector.begin(), reward_vector.end(), *vit);
         if(it==reward_vector.end())
         {
             reward_vector.push_back(*vit);
         } 
     }  
 }

 void update_reward_vector_delete(set<V_T> _get_pattern)
 {
     vector_it it;
     VIT vit;
     for(vit = _get_pattern.begin(); vit!= _get_pattern.end();vit++)
     {
         it = find(reward_vector.begin(), reward_vector.end(), *vit);
         if(it!=reward_vector.end())
         {
             reward_vector.erase(it);
         } 
     }
 }

  void update_item_score(LATTICE_NODE* update_node,double increment)
  {
   
      set<V_T> _get_pattern;
      _get_pattern = (update_node->_pat)->get_data();
      string cc = (update_node->_pat)->canonical_code();
      //  cout << "update Pattern:"<<cc<<endl;
      VIT vit;
      for(vit = _get_pattern.begin(); vit != _get_pattern.end(); vit++)
      {
          double s = _pf->get_itemscore_pf(*vit);
          for(int i = 0; i < reward_vector.size(); i++)
          {   
              if(reward_vector[i] == *vit)
              {
                  
                  increment = 1.75;
                  cout << "increment = "<<increment <<endl;
              }
          }
 

          for(int i = 0; i < penalty_vector.size(); i++)
          {  
              if(penalty_vector[i] == *vit)
              {
              
                  increment = 0.57;
                  cout << "increment penaltied = " << increment <<endl;
                  // cout << *vit<<" :"<<s<<","=;
              }
          }
          _pf->update_itemscore_pf(*vit,increment);
           increment=1;
       }
  }

  double calculate_MHscore(LATTICE_NODE* LN)const
  {
      vector_it it;
      set<V_T>_get_pattern;
      double score,get_scr=1;
      int cls;
      _get_pattern = (LN->_pat)->get_data();
      VIT vit;
      int rewarded=0;
      int penaltied=0;
      for(vit = _get_pattern.begin(); vit!= _get_pattern.end();vit++)
      {
          double s = _pf->get_itemscore_pf(*vit);
          get_scr *= s;
      }

      return get_scr;
  }

  double calculate_Qscore(PAT *&P)
  {
      set<V_T> _get_pattern;
      _get_pattern = P->get_data();
      return _pf->find_NewQscore_pf(_get_pattern);
  }

  private:
  FREQ_CNT_MAP _freq_cnt;
  FREQ_CNT_MAP _feedback_cnt;
  NODE_MAP _node_map;
  PatternFactory<PAT>* _pf;
  int _maxiter;
  PAT* _last;
  vector<double> _belief_score; //vector storing belief score of all transaction
  int addrandomedge;
  int startrandomedgeflag;
  int numberoflatticenode;
  int previouslyreportsize;
  int numberof_feedback;
  int feed;
  vector<V_T> reward_vector;
  vector<V_T> penalty_vector;
  ofstream alpha;
   
};

#endif
