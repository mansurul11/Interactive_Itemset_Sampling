//============================================================================
// Name        : interactive_pattern_mining.cpp
// Author      : /*
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "database.h"
#include <vector>
#include "random.h"

#include "random_walk_manager_freq_randomedge.h"

using namespace std;

char* datafile;
int minsup;
int uniq_pat_count;
int iter;
int feedb;

typedef Pattern<int> PAT;
template<> vector<int> Database<PAT>::_no_data = Database<PAT>::set_static_data();
template<> PatternFactory<PAT>* PatternFactory<PAT>::_instance = PatternFactory<PAT>::set_static_data();

void print_usage(char *prog) {
  cerr<<"Usage: "<<prog<<" -d data-file -c count -s minsup -f feedback"<<endl;
  exit(0);
}

void parse_args(int argc, char* argv[]) {
  if(argc<7) {
    print_usage(argv[0]);
  }

  for (int i=1; i < argc; ++i){
    if (strcmp(argv[i], "-d") == 0){
      datafile=argv[++i];
    }
    else if (strcmp(argv[i], "-s") == 0){
      minsup=atoi(argv[++i]);
      cout << "Minimum Support:" << minsup << " " << endl;
    }
    else if(strcmp(argv[i],"-c") == 0){
      iter=atoi(argv[++i]);
    }
	else if(strcmp(argv[i],"-f") == 0){
      feedb=atoi(argv[++i]);
    }
    else{
      print_usage(argv[0]);
    }
  }
}//end parse_args()


int main(int argc, char *argv[]) {

  parse_args(argc, argv);
  Database<PAT>* database;


  /* creating database and loading data */
  try {
    database = new Database<PAT>(datafile);
    database->set_minsup(minsup);
  }
  catch (exception& e) {
    cout << e.what() << endl;
  }
   cout<< "no of transaction=" << database->size();


  database->remove_infrequent_vertices(); // Remove infrequent items (lower than minimum support threshold)

  database->get_single_item_summary();// Gather Single item sets summary
  database->initialize_item_score();// Initialize the score of each frequent item i.e. initialization of sampling distribution
  RandomWalkManager_Freq<PAT> rwm(database, iter,feedb);
  lattice_node<PAT>* start = rwm.initialize();
  
  int cur_iter=0;
  rwm.walk(start, cur_iter);
  delete database;
}
