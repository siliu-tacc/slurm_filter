#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/stat.h>
#include <sstream>
#include <assert.h>
#include <parser_input.h>

using namespace std;

// An oldie, but a goodie: limit number of simultaneous jobs by a user

void vmcalo_rule(string User, string Queue, Parser_Input *iParse)
{

  printf("--> Enforcing max jobs per user...");

  int iret;
  int max_jobs_per_user;
  int jobs_per_exempt_user;
  string check_command;
  string keyword("tacc_filter/max_jobs/total_user_jobs");

  iret *= iParse->Read_Var("tacc_filter/max_jobs/total_user_jobs",&max_jobs_per_user);
  iret *= iParse->Read_Var("tacc_filter/user_job_binary",&check_command);

//  cout<<"MAX job per user"<<max_jobs_per_user<<endl;
//  cout<<"Check Command"<<check_command<<endl;
  
  if(iret == 0)
    {
      printf("\n\nMax job configuration error. Please\n");
      printf("contact TACC consulting for assistance.\n\n");
      exit(1);
    }
  else
    {

      assert(max_jobs_per_user > 0);

      // Check for possible (optional) exemption value for this user

      if(iParse->Read_Var(keyword + "/" + User,&jobs_per_exempt_user));
	{
	  if(jobs_per_exempt_user > 0)
	    max_jobs_per_user = jobs_per_exempt_user;
	}

      bool cmd_is_executable = false;
      struct stat  st;

      if (stat(check_command.c_str(), &st) == 0)
	if ((st.st_mode & S_IEXEC) != 0)
	  cmd_is_executable = true;

      if(! cmd_is_executable)
	{
	  printf("\n");
	  printf("\nJob submission filter configuration error for job check. Please\n");
	  printf("contact TACC consulting for assistance.\n\n");
	  exit(1);
	}

      stringstream full_command;

      full_command << check_command;
      full_command << " " << User;
      full_command << " " << max_jobs_per_user;

      int cmd_return = system(full_command.str().c_str());

      if(cmd_return != 0 )
	{
	  printf("FAILED\n\n");
	  printf("    [*] Too many simultaneous jobs in queue.\n");
	  printf("        --> Max job limits for %s =  %2i jobs\n\n",User.c_str(),max_jobs_per_user);
	  exit(1);
	}
      else
	  printf("OK\n");

    }

  return;
}


