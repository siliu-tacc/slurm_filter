// SLURM Plugin: simple welcome message header on job submission

#include <stdio.h>
#include <slurm/spank.h>
#include <stdlib.h>
//#include <unistd.h>

SPANK_PLUGIN(tacc_deny, 1)

//extern char **environ;

//int slurm_spank_init_post_opt (spank_t sp, int ac, char **av)
int slurm_spank_init_post_opt (spank_t sp, int ac, char **av)
{
  const char *queue;
  const char *jobid1, *jobid2;
  const char *command;

  queue=getenv("SLURM_QUEUE");
  jobid1=getenv("SLURM_JOBID");
  jobid2=getenv("SLURM_JOB_ID");
  command=getenv("SLURM_COMMAND");

  //This is the debug information!
  //printf(" QUEUE: %s\n JOBID is: %s\n JOB_IS is: %s\n COMMAND: %s\n",queue, jobid1, jobid2, command);

   //if sbatch is launched on compute node:
   if(command!=NULL)
	if(strcmp(command,"sbatch")==0)
	  {
          printf("\n");
          printf("[TACC]: Job submission is not allowed from this host. Please submit\n");
          printf("[TACC]: through one of the available login resources.\n");
          printf("\n");
          exit(1);
   	  return -1;
          }

  //if you are in the srun command and you have a jobid, it is OK
  if(command!=NULL)
    {
    //if the current one goes to any existing job, it will be allowed!
    if( (strcmp(command,"srun")==0) && ((jobid1!=NULL) || (jobid2!=NULL)) )
        {
        return 0;
        }
    else
        {
          printf("\n");
          printf("[TACC]: Job submission is not allowed from this host. Please submit\n");
          printf("[TACC]: through one of the available login resources.\n");
          printf("\n");
          exit(1);
        }
    }

    //srun(step)  will reset the SLURM_QUEUE to empty, so this one pass
//    if(queue==NULL)
//        {
//        return 0;
//        } 

  return 0;
}




