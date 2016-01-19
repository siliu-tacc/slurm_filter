// -*-c++-*-
//-----------------------------------------------------------------------
// TACC SLURM Plugin
// 
// SPANK plugin for use with SLURM at job submission time. Used to
// impose queue ACLs, verify file system availability, validate ssh
// keys, and various other duties as required for TACC productino
// environment.  Currently requires patch to SLURM in order to
// expose certain job submission details to the SPANK plugin
// environment.
// 
// Originally: 9/15/2012 (k.schulz)
//-----------------------------------------------------------------------
// $Id:$
//-----------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parser_input.h>

#ifdef __cplusplus

using namespace std;

void check_all_queue_limits        (const char *user, const char *queue, Parser_Input iparse);
void check_jobname_string          (const char*jobname_string);
void check_valid_ssh_keys          (string User);
void enforce_queue_limits          (string User, string Queue, int cores, int nodes, 
				    int runlimit, Parser_Input *iParse);
void enforce_accounting            (string User, string Queue, int cores, int nodes, string Project, 
				    int runlimit,Parser_Input *iparse);
void verify_filesystem_availability(const char *fs_name, const char *env_name);
void verify_queue_access           (std::string User, std::string Queue, Parser_Input *iParse);
void verify_submit_host            (Parser_Input *iparse);
void vmcalo_rule                   (string User, string Queue, Parser_Input *iParse);
void reservation_match_queue(const char* reservation, const char* queue_string, Parser_Input *iParse);


#endif


