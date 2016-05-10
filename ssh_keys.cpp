#include <string>
#include <sstream>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <fstream>
#include <iostream>
#include "tacc_submission_filter.h"

using namespace std;

// check HOME directory permission
// check .ssh directory permission

// check id_rsa.pub permission
// check id_rsa     permission

// checks ~username/.ssh/authorized_keys and
// ~username/.ssh/authorized_keys2 files for the public key in
// ~username/.ssh/id_rsa.pub. 
// 
// rewritten by Si Liu (04-14-2016)

// This version contains almost all basic tests
// Similar to what we get in the current sanitytool 1.3

void check_valid_ssh_keys(string username)
{

  struct stat file_stat;
  bool failed = false;

  //Start the test
  printf("--> Verifying valid ssh keys...");



  //Get the user info from /etc/passwd
  struct passwd *pw=getpwnam(username.c_str()); 
  if (pw == NULL)
    {
      printf("FAILED\n\n\n");
      printf("User %s does not exist\n",username.c_str());
      failed = true;
    }



  //Home directory
  //Home directory permission ok?
  string my_home = pw->pw_dir;
  ifstream home_dir(my_home.c_str());

  if (!failed)
  if (home_dir.fail())
    {
      printf("FAILED\n\n\n");
      printf("Failed to access HOME directory.\n");
      failed = true;
    }

  stat(my_home.c_str(), &file_stat);
  if (!failed)
  if ( (file_stat.st_mode & (S_IWGRP | S_IWOTH)) | !(file_stat.st_mode & S_IRUSR) | !(file_stat.st_mode & S_IWUSR) | !(file_stat.st_mode & S_IXUSR) )
  {
      printf("FAILED\n\n\n");
      printf("HOME directory does not have the proper permission!\n");
      failed = true;
  }



  //.ssh directory
  //.ssh directory permission ok?
  string my_ssh = pw->pw_dir;
  my_ssh+="/.ssh";
  ifstream ssh_dir(my_ssh.c_str());

  if (!failed)
  if (ssh_dir.fail())
    {
      printf("FAILED\n\n\n");
      printf("Failed to access .ssh directory.\n");
      failed = true;
    }

  stat(my_ssh.c_str(), &file_stat);
  if (!failed)
  if ( (file_stat.st_mode & (S_IWGRP | S_IWOTH)) | !(file_stat.st_mode & S_IRUSR) | !(file_stat.st_mode & S_IWUSR) | !(file_stat.st_mode & S_IXUSR) )
  {
      printf("FAILED\n\n\n");
      printf(".ssh directory does not have the proper permission!\n");
      failed = true;
  }




  //Public key file exist?
  //Public key file permission OK?
  string pub_key_file = pw->pw_dir;
  pub_key_file += "/.ssh/id_rsa.pub";  
  ifstream pub_key(pub_key_file.c_str());

  if (!failed)
  if (pub_key.fail())
    {
      printf("FAILED\n\n\n");
      printf("Failed to find public key file: %s\n",pub_key_file.c_str());
      failed = true;
    }

  //R and X permission from the user
  //No W and X permission from group or others
  stat(pub_key_file.c_str(), &file_stat);
  if (!failed)
  if ( (file_stat.st_mode & (S_IWGRP | S_IXGRP | S_IWOTH | S_IXOTH) ) | !(file_stat.st_mode & S_IRUSR) | !(file_stat.st_mode & S_IWUSR) )
  {
      printf("FAILED\n\n\n");
      printf("Publick key file does not have the proper permission!\n");
      failed = true;
  }

  //private key file exist?
  string pri_key_file = pw->pw_dir;
  pri_key_file += "/.ssh/id_rsa";
  ifstream pri_key(pri_key_file.c_str());
  if (!failed)
  if (pri_key.fail())
    {
      printf("FAILED\n\n\n");
      printf("Failed to find private key file: %s\n",pub_key_file.c_str());
      failed = true;
    }

  stat(pri_key_file.c_str(), &file_stat);
  if (!failed)
  if ( (file_stat.st_mode & (S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH) ) | !(file_stat.st_mode & S_IRUSR ) | !(file_stat.st_mode & S_IWUSR )) 
  {
      printf("FAILED\n\n\n");
      printf("Private key file does not have the proper permission!\n");
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

   if(failed)
    {
      printf("\nIncorrect ssh configuration.\n");
      printf("Please re-configuration or contact TACC consulting for assistance.\n\n");
      exit(1);
    }


// look at the authorized_keys? files and 
// make sure the public key is there
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

      stat(auth_keys_file.c_str(), &file_stat);
      if (file_stat.st_mode & (S_IXUSR | S_IWGRP | S_IXGRP | S_IWOTH | S_IXOTH) | !(file_stat.st_mode & S_IRUSR ) | !(file_stat.st_mode & S_IWUSR ))
	{
	  printf("FAILED\n\n\n");
          printf("Authorized_key file does not have the right permission!\n");
          failed = true;
          exit(1);
       }

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
      printf("\n\nImproper ssh configuration!\n");
      printf("Please re-configuration or contact TACC consulting for assistance.\n\n");
      exit(1);
    }
  else
    {
      printf("OK\n");
    }

  return;

}
