#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <parser_input.h>
#include "utils.h"

using namespace std;

void stop_on_error()
{
  printf("\n");
  printf("\nJob submission filter configuration error for accounting check. Please\n");
  printf("contact TACC consulting for assistance.\n\n");
  exit(1);
}

void enforce_accounting(string User, string Queue, int cores, int nodes, string Project, 
			int runlimit, Parser_Input *iParse)
{

//Debugging purposes
//printf("My Project: %s " , Project.c_str());

  printf("--> Checking available allocation ");

  if( !Project.empty() )
    printf("(%s)...",Project.c_str());

  fflush(NULL);

  string acct_cmd;
  string bypass_account;

  if(iParse->Read_Var("tacc_filter/accounting_check_binary",&acct_cmd ) )
    {

      // allow for possible accounting bypass

      std::string user_bypass = "tacc_filter/accounting_bypass/" + User;

      if(iParse->Read_Var(user_bypass.c_str(),&bypass_account) )
	{
	  if(bypass_account == Project)
	    {
	      printf("OK (BYPASS GRANTED)\n");
	      return;
	    } 
	  else {
	    printf("FAILED (BYPASS INVALID)\n");
	    printf("\nPlease verify correct project string or contact TACC consulting.\n\n");
	    exit(1);
	    return;
	  }
	}

      // verify binary existence

      struct stat  st;
      bool cmd_is_executable = false;

      if (stat(acct_cmd.c_str(), &st) == 0)
	if ((st.st_mode & S_IEXEC) != 0)
	  cmd_is_executable = true;

      if(! cmd_is_executable)
	stop_on_error();

      // Create tmp path for accounting filter output

      char newdir[] = "/tmp/tacc_acctXXXXXX";
      if (create_unique_dir(newdir) != 0)
	stop_on_error();

      string command_results(newdir);
      command_results += "/output";

      stringstream check_command;

      check_command << acct_cmd;
      check_command << " " << nodes;
      check_command << " " << cores;
      check_command << " " << runlimit;         // in seconds
      check_command << " " << User;
      check_command << " " << Queue;
      check_command << " " << Project;
      check_command << "> " << command_results;

//    printf("executing %s\n",check_command.str().c_str());
      
      int cmd_return = system(check_command.str().c_str());

      if(cmd_return != 0)
	{
	  printf("FAILED\n");

	  // show results from accounting filter

	  ifstream ifs ( command_results.c_str() , ifstream::in );

	  char myline[512];

	  while (ifs.getline(myline,512))
	    cout << myline << endl;

	  // clean-up
	  
	  unlink(command_results.c_str());
	  rmdir(newdir);

	  exit(1);
	}
      else
	{
	  // if user only has one project and did not specify at job
	  // submission time, then the accounting filter should give
	  // us back the correct project. Let's check.

	  if(Project.empty())
	    {
	      FILE *fp = fopen(command_results.c_str(),"r");
	      char project[80];	// max TAS projec length is 20

	      if(fp == NULL)
		stop_on_error();
	      
	      fscanf(fp,"%s",project);
	      printf("(%s)...",project);
	      fclose(fp);

	      if(strcmp(project,"") == 0)
		stop_on_error();

	      // Stash vetted allocation to env so that we can see it
	      // in slurm proper

	      setenv("TACC_OVERRIDE_PROJECT",project,1);

	    }

	  unlink(command_results.c_str());
	  rmdir(newdir);

	  printf("OK\n");

	}
    }
    else
      {
	stop_on_error();
      }

  return;
}


