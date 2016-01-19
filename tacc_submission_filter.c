// -*-c++-*-
//-----------------------------------------------------------------------
// TACC SLURM Plugin
// 
// SPANK plugin for use with SLURM at job submission time. Used to
// impose queue ACLs, verify file system availability, validate ssh
// keys, and various other duties as required for TACC productino
// environment.  Currently requires patch to SLURM submissions commands 
// in order to expose certain job submission details to the SPANK plugin
// environment.
// 
// Originally: 9/15/2012 (k.schulz)
//-----------------------------------------------------------------------
// $Id:$
//-----------------------------------------------------------------------

//Redesign the work for Wrangler and Lonestar 5 
//Si Liu on Sep 18

#include <slurm/spank.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#if 1
extern char **environ;
#endif

void driver (int argc, char *argv[]);

SPANK_PLUGIN(tacc_submission_filter, 1)

int slurm_spank_init_post_opt (spank_t sp, int ac, char **av)
{

#if 0
  char **p = environ;

  while (*p != NULL)
    {
      printf("%s (%p)\n", *p, *p);
      *p++;
    }
#endif

#if 0
  int NCPUS;
  uint16_t ncpus;
  spank_err_t rc;

  char myvar[512];
  sprintf(myvar,"koomie");
  rc = spank_job_control_getenv(sp,"SLURM_NNODES",myvar,100);
  printf("myvar = %s\n",myvar);
  printf("rc for env = %i\n",rc);
#endif

  // Derive necessary job submission elements

  //  if( spank_get_item (sp, S_JOB_NCPUS, &ncpus) == ESPANK_SUCCESS )
  //  if( spank_get_item (sp, S_JOB_UID, &foo) == ESPANK_SUCCESS )
  //  rc = spank_get_item (sp, S_JOB_NCPUS, &ncpus);

#if 0
  rc = spank_get_item (sp,  S_JOB_LOCAL_TASK_COUNT, &ncpus);
  if(rc != ESPANK_SUCCESS)
    printf("rc = %i\n",rc);
#endif

  //  printf("ncpus = %i\n",ncpus);
#if 0
    {
      NCPUS = 1;
    }
  else
    {
      printf("Unable to determine number of cores requested ...(job submission aborted)\n");
      exit(1);
    }
#endif

  // Call main driver (a C++ routine for convenience); avoids name
  // mangling issues with splank header and plugin loading

  driver(ac,av);

  return 0;

}



