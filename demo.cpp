#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

#include "tacc_submission_filter.h"
#include "parser_input.h"
#include "utils.h"

using namespace std;

int main (int argc, char *argv[])
{
  printf("Start the program!");
  string User("rubenv");
  string Queue("normal");
  string Project("071859800728");
  int cores(24);
  int nodes(1);
  int runlimit(900);

  printf("My Project: %s " , Project.c_str());
  printf("--> Checking available allocation ");

  Parser_Input iParse;

}
