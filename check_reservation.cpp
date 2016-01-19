#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "tacc_submission_filter.h"

using namespace std;

// ---------------------------------------------------
// This routine is used to check the reservation name and job name
// They should match somehow 
// Right now, this is only necessary for the Wrangle machine
//
// AuthorL Si Liu (Texas Advanced Computing Center)
// Last Modified: Sep 24, 2015
// ---------------------------------------------------
//
bool isPrefix(string prefix, string parent)
{
  if (prefix.empty())
    return true;
  if (parent.empty())
    return false;
  
  pair<string::iterator,string::iterator> res;
  res = std::mismatch(prefix.begin(), prefix.end(), parent.begin());

  if (res.first == prefix.end())
  {
    return true;
  }
  return false;
}

void reservation_match_queue(const char* reservation_string, const char* queue_string, Parser_Input *iParse)
{
  //The maximum number of allowalble queue-reservation prefix
  const int max_prefix_count=50;    
  //Vector Prefix list
  vector<std::string> Prefix_list;

  string Queue=queue_string;
  string Reservation=reservation_string;

//cout<<"Current Queue: "<<Queue<< endl;
//cout<<"Current Reservation: "<<Reservation<< endl;
  
  printf("--> Verifying valid queue and reservation...");
  
  string path = "tacc_filter/reservation/prefix_list";
  string prefix_string;

  for(int i=0;i<max_prefix_count;i++)
    {
      if(! iParse->Read_Var_iVec(path.c_str(),&prefix_string,i) )
        break;
      else
        {
          Prefix_list.push_back(prefix_string);
        }
    }

   for (vector<string>::iterator it = Prefix_list.begin(); it != Prefix_list.end(); it++) 
     {
	 if (isPrefix(*it, Queue) != isPrefix(*it, Reservation) )
	     {
	     cout<<endl<<endl<<"Queue ("<<Queue<<") and Reservation("<<Reservation<<") should match!"<<endl;
             exit(1);
	     }
         //cout <<"String in prefix list: " << *it << endl; 
     }


  printf("OK\n");
  return;
}

