#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <libgen.h>
#include <stdlib.h>

using namespace std;

  int CheckDir(const char *dirname);
  int verify_string_ends_with_6_Xs(char *name_template);
/*extern "C"*/ int check_file_path(const char *pathname);
/*extern "C" */int create_unique_dir(char *name_template);

