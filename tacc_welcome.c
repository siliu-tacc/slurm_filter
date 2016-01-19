// SLURM Plugin: simple welcome message header on job submission

#include <stdio.h>
#include <slurm/spank.h>

SPANK_PLUGIN(tacc_welcome, 1)

int slurm_spank_init (spank_t sp, int ac, char **av)
{

  printf("\n-----------------------------------------------------------------\n");
  printf("          Welcome to the Lonestar 5 Supercomputer          \n");
  printf("-----------------------------------------------------------------\n\n");

  return (0);
}




