#include <string>
#include <sstream>
#include <list>
#include <sys/types.h>
#include <pwd.h>
#include <fstream>
#include <iostream>
#include "tacc_submission_filter.h"

using namespace std;

// checks ~username/.ssh/authorized_keys and
// ~username/.ssh/authorized_keys2 files for the public key in
// ~username/.ssh/id_rsa.pub. 
// 
// modified via code from bbarth (12-9-2013:PM19)

// This version contains the basic tests. 
// More tests may be added from the sanitytool. 

void check_valid_ssh_keys(string username)
{

  printf("--> Verifying valid ssh keys...");
  bool failed = false;

  struct passwd *pw=getpwnam(username.c_str()); // Get the user info from /etc/passwd
  if (pw == NULL)
    {
      printf("FAILED\n\n\n");
      printf("User %s does not exist\n",username.c_str());
      failed = true;
    }
  
  string pub_key_file = pw->pw_dir;
  pub_key_file += "/.ssh/id_rsa.pub";  
  ifstream pub_key(pub_key_file.c_str());
  if (pub_key.fail())
    {
      printf("FAILED\n\n\n");
      printf("Failed to find public key file: %s\n",pub_key_file.c_str());
      failed = true;
    }

  // Find the public key text
  // (This test assumes the file has not been messed with too much)

  string pk;
  bool found=false; 
  while (getline(pub_key,pk))
    {
      size_t loc=pk.find("ssh-rsa");
      if(loc!=string::npos)
        {
          found=true;
          break;
        }
    }

  if(!found)
    {
      printf("FAILED\n\n\n");
      printf("File %s does not contain a public key\n",pub_key_file.c_str());
      failed = true;
    }

  list<string> auth_names;
  auth_names.push_back(string("authorized_keys"));
  auth_names.push_back(string("authorized_keys2"));

  bool found_ak=false; // Need to check for both ak files and flag if neither is present

  failed=true; // Assume not found until we do
  for(list<string>::iterator it=auth_names.begin();
      it != auth_names.end(); ++it)
    {
      // Build authorized_keys file name and check for existence
      string auth_keys_file=pw->pw_dir;
      auth_keys_file += "/.ssh/" + *it;
      ifstream auth_keys(auth_keys_file.c_str());
      if (auth_keys.fail())
        continue;
      else
        found_ak=true;

      // Directly compare lines in authorized key file to found public key
      string ak;
      while(getline(auth_keys,ak))
        if (ak.compare(pk) == 0) // works like strncasecmp (== 0 is success)
	  {
	    failed=false;
	    break;
	  }
    }

  if(!found_ak)
    {
      printf("FAILED\n\n\n");
      printf("Failed to find valid authorized_keys or authorized_keys2 files\n");
      failed = true;
    }
  
  if(failed)
    {
      printf("\n\nUnable to verify ssh configuration. Please update your ssh key\n");
      printf("configuration or contact TACC consulting for assistance.\n\n");
      exit(1);
    }
  else
    printf("OK\n");

  return;
}
