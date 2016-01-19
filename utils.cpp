#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <libgen.h>
#include <stdlib.h>
#include "utils.h"

#ifndef UTILS
#define UTILS

using namespace std;

  int CheckDir(const char *dirname)
  {
    struct stat st;

    if(stat(dirname,&st) != 0)
      {
        if( mkdir(dirname,0700) != 0 )
          {
            fprintf (stderr, "unable to create directory %s", dirname);
            return -1;
          }
      }
    else if (!S_ISDIR(st.st_mode))
      {
        fprintf (stderr, "entry exists but is not a directory %s",dirname);
        return -1;
      }

    return 0;
  }

  int verify_string_ends_with_6_Xs(char *name_template)
  {
    const int NUM_REQUIRED_TRAILING_X = 6;
    int length_template;
    
    if(name_template == NULL)
      {   
        fprintf(stderr, "Invalid directory template name (null) ");
        return 1;
      }   

    length_template = strlen(name_template);
    
    for(int i=length_template-NUM_REQUIRED_TRAILING_X;i<length_template;i++)
      {   
        if(name_template[i] != 'X')
          {   
            fprintf(stderr, "Invalid directory template (must end with XXXXXX) - %s",name_template);
            return 1;
          }   
      }   
    
    return 0;
  }


int check_file_path(const char *pathname)
  {
    const int MAX_DEPTH = 50;

    char *pathlocal;
    char *parents;
    char *dirstring;
    char *token;
    int depth = 0;

    // Save a copy of pathname and look for the parent directories.

    pathlocal = strdup(pathname);
    dirstring = strdup(pathname);
    parents   = dirname(pathlocal);

    if(strcmp(parents,".") == 0)
      { 
        free(pathlocal);
        free(dirstring);
        return 0;
      }

    // Deal with the possibility of an absolute path being provided

    bool abs_path = false;

    std::string leading_char("");
    std::string path_to_check;

    if(strncmp(parents,"/",1) == 0)
      {
        leading_char = "/";
        abs_path     = true;
      }

    // Verify existence of top-level directory

    if( (token = strtok(parents,"/")) != NULL )
      {
        path_to_check += leading_char + token;

        if ( CheckDir(path_to_check.c_str()) )
          {
            free(pathlocal);
            free(dirstring);
            return -1;
          }

        // Now, search for any remaining parent directories.

        if(abs_path)
          sprintf(dirstring,"/%s",token);
        else
          sprintf(dirstring,"%s",token);

        while ( (token = strtok(0,"/")) && (depth < MAX_DEPTH) )
          {
            dirstring = strcat(dirstring,"/");

            if(CheckDir(strcat(dirstring,token)))
              {
                free(pathlocal);
                free(dirstring);
                return -1;
              }
            depth++;
          }
        if(depth >= MAX_DEPTH )
          {
            fprintf(stdout,"Max directory depth exceeded, limit = %d",MAX_DEPTH);
            free(pathlocal);
            free(dirstring);
            return -1;
          }
      }

    // Clean Up
    free(pathlocal);
    free(dirstring);

    return 0;
  }

int create_unique_dir(char *name_template)
  {

    int status = verify_string_ends_with_6_Xs(name_template);
    if (status) return status;

    if (mkdtemp(name_template) == NULL)  /* mkdtemp sets errno */
      return -1;
    else
      return 0;
  }

#endif
