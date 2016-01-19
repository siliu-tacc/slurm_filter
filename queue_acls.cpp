#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "tacc_submission_filter.h"

using namespace std;

void verify_queue_access(string User, string Queue, Parser_Input *iParse)
{

  // Any public/private ACL defined for queue?

  printf("--> Verifying access to desired queue (%s)...",Queue.c_str());

  bool is_public  = false;
  bool is_private = false;

  vector<std::string> acl_public;
  vector<std::string> acl_private;
  std::string acl;
  std::string user_message;

  string prefix_public  = "tacc_filter/queue_acls/public/";
  string prefix_private = "tacc_filter/queue_acls/private/" + Queue;

  const int max_acls_per_queue = 1024;

  // --------------------------------------------
  // read user error message
  // --------------------------------------------

  if(iParse->Read_Var("tacc_filter/queue_acl_denial_message",&user_message) );

  // --------------------------------------------
  // check for any global ACLs for public queues
  // --------------------------------------------

  string path = prefix_public + "ALL";

  for(int i=0;i<max_acls_per_queue;i++)
    {
      if(! iParse->Read_Var_iVec(path.c_str(),&acl,i) )
	break;
      else
	{
	  acl_public.push_back(acl);
 	}
    }

  // check for any queue-specific ACLs for public queues

  path = prefix_public + Queue;

  for(int i=0;i<max_acls_per_queue;i++)
    {
      if(! iParse->Read_Var_iVec(path.c_str(),&acl,i) )
	break;
      else
	{
	  acl_public.push_back(acl);
	  is_public = true;
	}
    }

  // read private acls

  for(int i=0;i<max_acls_per_queue;i++)
    {
      if(! iParse->Read_Var_iVec(prefix_private.c_str(),&acl,i) )
	break;
      else
	{
	  acl_private.push_back(acl);
	  is_private = true;
	}
    }

  if(is_public && is_private)
    {
      printf("\n\nJob submission filter configuration error for public/private queues. Please\n");
      printf("contact TACC consulting for assistance.\n\n");
      exit(1);
    }

  // Enforce Blacklist (updated 3/22/13): if a user is specified in
  // the blackist, then they are denied from all queue submissions,
  // both public and private

  string blacklist_user         = "!";
  string blacklist_with_message = "!!";

  blacklist_user         += User;
  blacklist_with_message += User;
	    
  for(size_t i=0;i<acl_public.size();i++)
    if(acl_public[i] == blacklist_user ||
       acl_public[i] == blacklist_with_message )
      {
	printf("FAILED\n");
		
	if(acl_public[i] == blacklist_with_message)
	  {
	    printf("\n");
	    printf("%s\n",user_message.c_str());
	    printf("\n");
	  }
	exit(1);
      }

  // if the queue is not defined, we assume it is public and grant access

  if( !is_public && !is_private)
    {
      printf("OK\n");
      return;
    }
  else
    {
      if(is_private)		// private queue - user must be granted access
	{
	  for(size_t i=0;i<acl_private.size();i++)
	    if(acl_private[i] == User)
	      {
		printf("OK\n");
		return;
	      }

	  // user was not defined, disallow access

	  printf("FAILED\n");
	  exit(1);
	}
      else			// public queue -> grant access to those not already denied via blacklist
	{
	  printf("OK\n");
	  return;
	}
    }

  return;
}

