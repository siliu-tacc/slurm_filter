#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#include "tacc_submission_filter.h"

void verify_filesystem_availability(const char *fs_name, const char *env_name)
{
  const char *fs_path;

  char filepath[120];
  const char *tmp_file = ".slurm/tacc_test";

  if( (fs_path = getenv(env_name) ) == NULL) 
    {
      printf("\nUnable to query %s environment variable...(job submission aborted)\n",env_name);
      exit(1);
    }

  // Verify we can write to user's $HOME

  printf("--> Verifying availability of your %s dir (%s)...",fs_name,fs_path);

  if( strlen(fs_path) > (sizeof(filepath) + sizeof(tmp_file) - 3) )
    {
      printf("%s dir too long: FAILED\n",fs_name);
      exit(1);
    }

  if (sprintf(filepath,"%s/%s",fs_path,tmp_file) < 0)
    {
      printf("error forming test file string: FAILED\n");
      exit(1);
    }

  if( check_file_path(filepath) != 0)
    {
      printf("FAILED\n");
      exit(1);
    }

  // print to file to verify write capability

  FILE *fp_test = fopen(filepath,"w");

  if(fp_test == NULL)
    {
      printf("FAILED\n");
      exit(1);
    }

  if (fprintf(fp_test,"The answer is %i\n",42) < 0 )
    {
      fclose(fp_test);
      printf("FAILED\n");
      exit(1);
    }
  else 
    {
      fclose(fp_test);
      printf("OK\n");
    }

  return;

}

