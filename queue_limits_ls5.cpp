#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <parser_input.h>
#include <sys/stat.h>
#include <sstream>
#include <math.h>

using namespace std;

void enforce_queue_limits(string User, string Queue, int cpus, int nodes, int runlimit_secs, Parser_Input *iParse)
{
  int cores_per_node;
  int max_cores_allowed;
  int max_hours_allowed;

  int max_cores_allowed_user;
  int max_hours_allowed_user;

  int num_user_jobs;
  int max_jobs_per_queue;
  string check_command;

  int iret = 1;

  string expire_date;
  bool expiration_enforced = false; // has an expiration date been provided for user override?
  bool override_allowed    = true;  // is override allowed (ie. it has not been expired)?

  char buf[80];
  struct tm tstruct;
  time_t now = time(0);
  tstruct    = *localtime(&now);

  printf("--> Verifying job request is within current queue limits...");

  // Default for LS5
  cores_per_node = 48;

  if (Queue == "gpu" || Queue =="vis")
    cores_per_node = 20;


  // Only for VM test
  // Remember to remove it for productive machines
  // cores_per_node=1;

  int total_cores;

  if((nodes < 1) || (Queue=="grace-serial") )
    total_cores = cpus;
  else
    total_cores = cores_per_node*nodes;

  if(cpus > total_cores)
    {
      printf("FAILED\n\n\n");
      printf("  Requested too many cores/node\n");
      exit(1);
    }

  // default options - apply to all users

  iret *= iParse->Read_Var("tacc_filter/queue_size_limits/" + Queue + "/max_cores",&max_cores_allowed);
  iret *= iParse->Read_Var("tacc_filter/queue_size_limits/" + Queue + "/max_hours",&max_hours_allowed);

  // check for user-specific expiration date (enforced for any user-specific override options)

  std::string user_expire_key="tacc_filter/queue_size_limits/" + Queue + "/" + User + "_expire";

  if(iParse->Read_Var(user_expire_key.c_str(),&expire_date) )
    expiration_enforced = true;

  // Note on implementation logic: we only want to use override
  // options if the user request exceeds the default values;
  // otherwise, we just revert to the default settings.

  if(expiration_enforced)
    {
      strftime(buf,sizeof(buf),"%Y-%m-%d %X",&tstruct);

      string expired = expire_date + " 23:59:59";
      string current = buf;
      #if DEBUG
      printf("Current    date = %s\n",current.c_str());
      printf("Expiration date = %s\n",expired.c_str());
      #endif

      if(current <= expired)
      {
          override_allowed = true;
          #if DEBUG
          printf("within valid override time\n");
          #endif
      }
      else
      {
          override_allowed = false;
          #if DEBUG
          printf("override time expired\n");
          #endif
      }
    }
  else
    override_allowed = true;

  // check for any user-specific overrides (if request exceeds default settings)

  if(total_cores > max_cores_allowed || (float(runlimit_secs/3600.0) > max_hours_allowed) )
    if(override_allowed)
      {
	if(iParse->Read_Var("tacc_filter/queue_size_limits/" + Queue + "/" + User + "_max_cores" ,
			    &max_cores_allowed_user))
	  max_cores_allowed = max_cores_allowed_user;
	
	if(iParse->Read_Var("tacc_filter/queue_size_limits/" + Queue + "/" + User + "_max_hours" ,
			    &max_hours_allowed_user))
	  max_hours_allowed = max_hours_allowed_user;
      }

  if(iret == 0)
    {
      printf("\n\nNo queue limits available (queue=%s).\n",Queue.c_str());
      printf("\nPlease verify that you submitted to a valid queue or contact TACC \n");
      printf("consulting for assistance.\n\n");
      exit(1);
    }
  else
    {
      if(total_cores > max_cores_allowed || (float(runlimit_secs/3600.0) > max_hours_allowed) )
	{
//	  cout<<"total cores: "<<total_cores<<endl;
//	  cout<<"max_cores_allowed: "<<max_cores_allowed<<endl;
          printf("FAILED\n\n");
	  printf("You are requesting %i Physical cores on %.0f nodes for %.2f hours!\n", total_cores/2, ceil(total_cores*1.0/cores_per_node),runlimit_secs/3600.0);
	  printf("  The Requested job is not within current queue limits:\n");
	  printf("  --> %s Queue Limits: %i Hours\n",Queue.c_str(),max_hours_allowed);
	  printf("  --> %s Queue Limits: %i(%i) Virtual(Physical) cores on %.0f Nodes\n",Queue.c_str(), max_cores_allowed, max_cores_allowed/2,ceil(max_cores_allowed*1.0/cores_per_node));
	  exit(1);
	}

    }
  #if DEBUG
    printf("checking for queue_size_limits (%s\n)",Queue.c_str());
  #endif
  if(iParse->Read_Var("tacc_filter/queue_size_limits/" + Queue + "/max_jobs" ,
		      &max_jobs_per_queue))
    {
      // Queue has a max number of jobs limit set, check if user has too many in the queue
        #if DEBUG
            printf("we have a winner\n");
        #endif

      iret = iParse->Read_Var("tacc_filter/user_job_binary",&check_command);
      bool cmd_is_executable = false;
      struct stat  st;

      if (stat(check_command.c_str(), &st) == 0)
	if ((st.st_mode & S_IEXEC) != 0)
	  cmd_is_executable = true;

      if(! cmd_is_executable)
	{
	  printf("\n");
	  printf("\nJob submission filter configuration error for per queue job check. Please\n");
	  printf("contact TACC consulting for assistance.\n\n");
	  exit(1);
	}

      stringstream full_command;

      full_command << check_command;
      full_command << " " << User;
      full_command << " " << max_jobs_per_queue;
      full_command << " " << Queue;

      #if DEBUG
         printf(INFO,"command to run = %s\n",full_command.str().c_str());
      #endif

      int cmd_return = system(full_command.str().c_str());

      if(cmd_return != 0 )
	{
	  printf("FAILED\n\n");
	  printf("    [*] Too many simultaneous jobs in queue.\n");
	  printf("        --> Max job limits for %s =  %2i jobs\n\n",Queue.c_str(),max_jobs_per_queue);
	  exit(1);
	}

    }

  printf("OK\n");

  return;
}


