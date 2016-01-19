#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tacc_submission_filter.h"

// ---------------------------------------------------
// Disallow colons and single quotes in jobnames 
// (added 12-9-2013, PM19)
// ---------------------------------------------------

void check_jobname_string(const char*jobname_string)
{
  printf("--> Verifying valid jobname...");
  
  if( (jobname_string != NULL) )
    {
      if(strstr(jobname_string,":") != NULL)
	{
	  printf("FAILED\n\n\n");
	  printf("Job names are not allowed to contain the \":\" character.\n");
	  printf("--> submitted jobname = %s\n\n",jobname_string);
	  printf("Please remove and resubmit the job.\n\n");
	  exit(1);
	}
      if(strstr(jobname_string,"'") != NULL)
	{
	  printf("FAILED\n\n\n");
	  printf("Job names are not allowed to contain the \"'\" character.\n");
	  printf("--> submitted jobname = %s\n\n",jobname_string);
	  printf("Please remove and resubmit the job.\n\n");
	  exit(1);
	}
    }

  printf("OK\n");
  return;
}

