#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "tacc_submission_filter.h"

using namespace std;

void verify_submit_host(Parser_Input *iParse)
{

   char name[32] ;
   gethostname( name, 32 ) ;

   char *dot=NULL;

   if ( (dot = strchr(name,'.')) != NULL )
     *dot = '\0';

  string hostname = name;

  printf("--> Verifying valid submit host (%s)...",hostname.c_str());

  string sub_hosts;

  // We err on the side of caution here. If the user asked to restrict
  // submission hosts, but didn't give us any named hosts, we restrict
  // all jobs

  const int max_submit_hosts = 64;
  vector<std::string>  submit_hosts;
  
  for(int i=0;i<max_submit_hosts;i++)
    {
      std::string host;

      if(! iParse->Read_Var_iVec("tacc_filter/submission_hosts",&host,i) )
	break;
      else
	submit_hosts.push_back(host);
    }

  if(submit_hosts.size() == 0)
    {
      printf("FAILED\n");
      exit(1);
    }

  for(int i=0;i<submit_hosts.size();i++)
    {
//    cout<<"valid string "<< submit_hosts[i];
      if(submit_hosts[i] == hostname)
	{
	  printf("OK\n");
	  return;
	}
    }

  // If we made it here, the host was not found as a submission host

  printf("FAILED\n");
  exit(1);

  return;

}

