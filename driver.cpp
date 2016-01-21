#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <assert.h>
#include "tacc_submission_filter.h"
#include "parser_input.h"

using namespace std;

// Top-level driver for checking various site-enforced
// restrictions at job submission time.
extern "C" {
  void driver(int argc, char *av[])
{

  bool check_home;
  bool check_work;
  bool check_scratch;
  bool check_ssh_keys;
  bool check_queue_acls;
  bool check_queue_limits;
  bool check_queue_runlimits;
  bool check_jobname;
  bool limit_submission;
  bool check_accounting;
  bool restrict_max_jobs;
  bool check_reservation;

  // determine submission user and queue

  const char *user_string;             	// submitting user
  const char *queue_string;            	// submission queue
  const char *reservation_string;	//reservation name if any

  string User;
  string Queue;

  if( (user_string = getenv("USER") ) == NULL)
    {
      printf("Unable to determine user...(job submission aborted)\n");
      exit(1);
    }
  else
    User = user_string;

  // root is never allowed, let's check that right up front

  if ( User == "root" )
    {
      printf("User is not authorized for job submission\n");
      exit(1);
    }

  // Query remaining job submission variables we need starting with
  // queue name (required)

  if( (queue_string = getenv("SLURM_QUEUE") ) == NULL)
    {
      printf("--> Submission error: all jobs must have a queue name specified with \"-p\"\n\n");
      exit(1);
    }
  else
    Queue = queue_string;

  // Job size requirements
  
  char *cpu_set;
  char *node_set;
  char *cpu_string;
  char *nodes_string;

  // ------------------------
  // Job sizing information
  // ------------------------

  int cpus  = 0;
  int nodes = 0;

  cpu_set  = getenv("SLURM_TACC_NCORES_SET");
  cpu_string   = getenv("SLURM_TACC_CORES");

  node_set = getenv("SLURM_TACC_NNODES_SET");
  nodes_string = getenv("SLURM_TACC_NODES");

  // A user must specify ncores at a minumum

  if(cpu_set == NULL || node_set == NULL)
    {
      printf("Unable to query submission cores...(job submission aborted)\n");
    }

  if( strcmp(cpu_set,"0") == 0 )
    {
      printf("--> Submission error: please define total core count with the \"-n\" option\n");
      exit(1);
    }
  else
    {
      assert(cpu_string != NULL);
      cpus = atoi(cpu_string);
    }

  if( strcmp(node_set,"1") == 0)
    {
      assert(nodes_string != NULL);
      nodes = atoi(nodes_string);
    }


  // quick sanity check for cpus and nodes

  if( (cpus < 1) )
    {
      printf("--> Job submission error: processor/node request values must be > 0\n");
      exit(1);
    }

  // ------------------------
  // Job runlimit (required)
  // ------------------------



  int runlimit_mins = 0;
  int runlimit_secs = 0;

  char *time_string;

  time_string   = getenv("SLURM_TACC_RUNLIMIT_MINS");

  if(time_string == NULL)
    {
      printf("--> Submission error: all jobs must have a maximum runlimit defined with \"-t\"\n");
      exit(1);
    }
  else
    {
      runlimit_mins = atoi(time_string);
      runlimit_secs = runlimit_mins * 60;

      // interesting: have seen slurm give us a value of -2 at times
      // when a user doesn't specify 

      if(runlimit_secs <= 0)
	{
	  printf("--> Submission error: all jobs must have a maximum runlimit defined with \"-t\"\n");
	  exit(1);
	}
    }

  // ------------
  // Job account 
  // ------------

  char *act_string = NULL;
  string Account("");

  act_string   = getenv("SLURM_TACC_ACCOUNT");
  //if( (act_string != NULL) || (strlen(act_string) == 0) )


//This part is necessary for Stampede, Lonestar 5
//The Wrangler setting is different
  if( (act_string != NULL) )
    {
      //printf("we got %p, size = %i\n",act_string,strlen(act_string));
      //strcpy(act_string,Account.c_str());
      Account = act_string;
    }



  // ------------
  // Job name
  // ------------

  char *jobname_string = NULL;

  jobname_string   = getenv("SLURM_TACC_JOBNAME");


  // ------------------------------
  // load runtime options from file
  // ------------------------------

  Parser_Input iParse;

  if(argc >= 1)
    {
       #if DEBUG
          printf("[loading tacc filter options from %s]\n",av[0]);
       #endif
      if (! iParse.Open(av[0]) )
	{
	  printf("\nUnable to parse TACC job submission options file. Please contact TACC\n");
	  printf("consulting for assistance.\n\n");
	  exit(1);
	}
      
      // read runtime options: by default, we do nothing, much like in life

      iParse.Read_Var("tacc_filter/check_home",               &check_home,         false);
      iParse.Read_Var("tacc_filter/check_work",               &check_work,         false);
      iParse.Read_Var("tacc_filter/check_scratch",            &check_scratch,      false);
      iParse.Read_Var("tacc_filter/check_ssh_keys",           &check_ssh_keys,     false);
      iParse.Read_Var("tacc_filter/check_queue_acls",         &check_queue_acls,   false);
      iParse.Read_Var("tacc_filter/restrict_max_user_jobs",   &restrict_max_jobs,  false);
      iParse.Read_Var("tacc_filter/check_queue_runlimits",    &check_queue_limits, false);
      iParse.Read_Var("tacc_filter/check_accounting",         &check_accounting,   false);
      iParse.Read_Var("tacc_filter/restrict_submission_hosts",&limit_submission,   false);
      iParse.Read_Var("tacc_filter/check_jobname",            &check_jobname,      false);
      iParse.Read_Var("tacc_filter/check_reservation",        &check_reservation,      false);
    }
  else
    {
      printf("\nUnable to load TACC job submission options file. Please contact TACC\n");
      printf("consulting for assistance.\n\n");
      exit(1);
    }

    // -----------
    // Reservation if any
    // -----------
    if( (reservation_string = getenv("SLURM_TACC_RESERVATION") ) == NULL)
      {
       cout<<"No reservation for this job"<<endl;
       check_reservation=false;
      } 


  //  iParse.Fdump();

  // Perform desired job submission checks.

  if(limit_submission)
    verify_submit_host(&iParse);
  if(check_jobname)
    check_jobname_string(jobname_string);
  if(restrict_max_jobs)
    vmcalo_rule(User,Queue,&iParse);
  if(check_home)
      verify_filesystem_availability("home","HOME");
//  not necessary for VM
  if(check_work)
      verify_filesystem_availability("work","WORK");
  if(check_scratch)
     verify_filesystem_availability("scratch","SCRATCH");
  if(check_ssh_keys)
    check_valid_ssh_keys(User);
  if(check_queue_acls)
    verify_queue_access(User,Queue,&iParse);
  if(check_queue_limits)
    enforce_queue_limits(User,Queue,cpus,nodes,runlimit_secs,&iParse);
  if(check_accounting)
    enforce_accounting(User,Queue,cpus,nodes,Account,runlimit_secs,&iParse);
 if(check_reservation)
     reservation_match_queue(reservation_string, queue_string,&iParse);

  iParse.Close();

  return;
}

} // end extern "C"
