#include <stdio.h>
#include <stdlib.h>

int main()
{

  const char *null1;
  const char *null2;
  const char *null3;
  const char *null4;

  null1=getenv("SLURM_NOEXIST");

//null2='\0';
//null3="null";
//null4="\none";

//  if(strcmp(null1,"/none") == 0)
   if (null1==NULL)
	{
	printf("empty string!\n");
	return 0;
	}

//  if (null1[0]!='\0')
//  	printf ("null1 =%s\n", null1);
//  printf(" null1=%s\n null2=%s\n null3=%s\n null4=%s\n",null1, null2, null3, null4);

  return 0;

}

