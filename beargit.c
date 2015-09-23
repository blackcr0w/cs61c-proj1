#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - Here are some of the helper functions from util.h:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the project spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */
int current_file_no = 0;

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);

  FILE* fbranches = fopen(".beargit/.branches", "w");
  fprintf(fbranches, "%s\n", "master");
  fclose(fbranches);

  FILE* fprev = fopen(".beargit/.prev", "w");
  fprintf(fprev, "%s\n", "0000000000000000000000000000000000000000");
  fclose(fprev);

  FILE* f_curr_branch = fopen(".beargit/.current_branch", "w");
  fprintf(f_curr_branch, "%s\n", "master");
  fclose(f_curr_branch);

  return 0;
}



/* beargit add <filename>
 *
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR:  File <filename> has already been added.
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {

  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w"); // jk: what is the diff of these two pointers?

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    current_file_no++;
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR:  File %s has already been added.\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      current_file_no--;
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index"); // move newindex to index, overwrite old one

  return 0;
}

/* beargit status
 *
 * See "Step 1" in the project spec.
 *
 */

int beargit_status() {
  /* COMPLETE THE REST */

  FILE* findex = fopen(".beargit/.index", "r");
  char line[FILENAME_SIZE];
  int i = 0;

  fprintf(stdout, "Tracked files:\n\n");
  while(fgets(line, sizeof(line), findex)) {
    fprintf(stdout, "%s", line);
    i++;
  }
  fprintf(stdout, "\n");
  fprintf(stdout, "There are %d files total.\n", i);
  fclose(findex);

  return 0;
}

/* beargit rm <filename>
 *
 * See "Step 2" in the project spec.
 *
 */

int beargit_rm(const char* filename) {  // need better implementation of beargit rm
  /* COMPLETE THE REST */
  //jk: need modify again

  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w"); // jk: what is the diff of these two pointers?

  char line[FILENAME_SIZE];
  int file_exist = 0;

  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      file_exist = 1;
      continue;     
    }
    fprintf(fnewindex, "%s\n", line);
  }

  if (file_exist) {
    // fs_rm(".beargit/.index");
    fclose(findex);
    fs_rm(".beargit/.index");
    FILE* findex2 = fopen(".beargit/.index", "w");  
    fclose(findex2);
    fclose(fnewindex);
    fs_cp(".beargit/.newindex", ".beargit/.index");
    fs_rm(".beargit/.newindex");
    return 0;
  }
  else{
    //fs_rm(".beargit/.newindex");
    fprintf(stderr, "ERROR:  File %s not tracked.\n", filename);
    fclose(fnewindex);
    return 1;
  }

}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the project spec.
 *
 */

 /*This is the helper function for compare two strings without library function.*/
int string_contains(const char *msg,const char *go_bears){
  if (strstr(msg, go_bears) != NULL)
    return 1;
  else 
    return 0;
/*    int i = 0,flag=0;
    size_t go_bears_len = strlen(str2);
   
    while(str1[i]!='\0' && str2[i]!='\0' && i < go_bears_len) {
         if(str1[i]!=str2[i]){
             flag=1;
             break;
         }
         i++;
    }

    if (flag== 0 && i >= go_bears_len)
         return 1;
    else
         return 0;*/
}

const char* go_bears = "THIS IS BEAR TERRITORY!";

int is_commit_msg_ok(const char* msg) {

  /* COMPLETE THE REST */
  int is_ok = string_contains(msg, go_bears);
  if (is_ok)
    return 1;  // match
  else {
    return 0;  // not match
  }  
}

/* Use next_commit_id to fill in the rest of the commit ID.
 *
 * Hints:
 * You will need a destination string buffer to hold your next_commit_id, before you copy it back to commit_id
 * You will need to use a function we have provided for you.
 */


void next_commit_id(char* commit_id) {
     /* COMPLETE THE REST */

  char dest_buf[SHA_HEX_BYTES + 1];
  cryptohash(commit_id, dest_buf);
  strcpy(commit_id, dest_buf);

}

// jk: TODO: using beargit.h to check error cases.
int beargit_commit(const char* msg) {
  if (strlen(msg) > MSG_SIZE)
    return 0;
  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR:  Message must contain \"%s\"\n", go_bears);  
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);

  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  char new_commit_id[COMMIT_ID_SIZE + BRANCHNAME_SIZE];
  strcpy(new_commit_id, commit_id);
  strcat(new_commit_id, current_branch);
  next_commit_id(new_commit_id);  // jk: core dumped here. fixed: set dest_buf an fixed array, no a pointer

/*  
  char *commit_id_char;
  strcpy(commit_id_char, commit_id);
  printf("create dir: %s\n", commit_id_char);*/

  char new_commit_dir[1000], new_commit_index[1000], new_commit_prev[1000];
  strcpy(new_commit_dir, ".beargit/");
  strcat(new_commit_dir, new_commit_id);
  strcat(new_commit_dir, "/");

  strcpy(new_commit_index, new_commit_dir);
  strcpy(new_commit_prev, new_commit_dir);
  strcat(new_commit_index, ".index");
  strcat(new_commit_prev, ".prev");


  fs_mkdir(new_commit_dir);
  FILE* findex2 = fopen(new_commit_index, "w");
  FILE* fprev = fopen(new_commit_prev, "w");
  fclose(fprev);
  fclose(findex2);

  fs_cp(".beargit/.index", new_commit_index);
  fs_cp(".beargit/.prev", new_commit_prev);

  char new_commit_msg[1000];
  strcpy(new_commit_msg, new_commit_dir);
  strcat(new_commit_msg, ".msg");
  FILE* fmsg = fopen(new_commit_msg, "w");
  fprintf(fmsg, "%s\n", msg);
  fclose(fmsg);
  FILE* fcommitid = fopen(".beargit/.prev", "w");
  fprintf(fcommitid, "%s\n", new_commit_id);
  fclose(fcommitid); 
  /* COMPLETE THE REST */
  //jk: printf("the next is: %s\n", current_branch);

  return 0;
}

/* beargit log
 *
 * See "Step 4" in the project spec.
 *
 */

int beargit_log(int limit) {
  /* COMPLETE THE REST */
  char *current_commit_dir = (char *) malloc(FILENAME_SIZE);  //jk: current working dir
  char *curr_dir = (char *) malloc(FILENAME_SIZE);  //jk: use this to read ID and msg
  char *next_dir = (char *) malloc(FILENAME_SIZE);  //jk: use this to read ID and msg  
  char *current_commit_id = (char *) malloc(COMMIT_ID_SIZE);  //jk: store ID
  char *current_commit_msg = (char *) malloc(MSG_SIZE);  //jk: stroe msg
  int number_of_commit;
  const char *prev = ".prev";
  const char *msg = ".msg";
  const char *beargit = ".beargit/";

  strcpy(curr_dir, beargit);
  number_of_commit = 1;

  while(number_of_commit <= limit) {
    // char *prev_dir = (char *) malloc(FILENAME_SIZE);
    // strcpy(curr_dir, current_commit_dir);
    char *temp_dir = (char *) malloc(FILENAME_SIZE);
    strcpy(temp_dir, curr_dir);
    strcat(temp_dir, prev);
    FILE* f_temp_dir = fopen(temp_dir, "r");
    char *current_commit_id = (char *)malloc(COMMIT_ID_SIZE);

    fscanf(f_temp_dir, "%s", current_commit_id);
    fclose(f_temp_dir);
    memset(temp_dir, '\0', sizeof(temp_dir));
   
    strcpy(temp_dir, beargit);
    strcat(temp_dir, current_commit_id);
    strcat(temp_dir, "/");
    strcpy(next_dir, temp_dir);

    if (number_of_commit == 1 && !fs_check_dir_exists(temp_dir)){
      fprintf(stderr, "%s\n", "ERROR:  There are no commits.");
      return 1;
    }

    if (!fs_check_dir_exists(temp_dir))
      return 0;

    strcat(temp_dir, msg);

  
    read_string_from_file(temp_dir, current_commit_msg, MSG_SIZE);

    printf("commit %s\n", current_commit_id);
    printf("   %s\n\n", current_commit_msg);

    memset(curr_dir, '\0', sizeof(curr_dir));
    strcpy(curr_dir, next_dir);
    free(temp_dir);
    memset(next_dir, '\0', sizeof(curr_dir));

    number_of_commit++;

    if (!fs_check_dir_exists(curr_dir))
      return 0;
  }
  free(current_commit_dir);
  free(curr_dir);
  free(next_dir);
  free(current_commit_id);
  free(current_commit_msg);

  return 0;
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
// jk: do not know how this is used.
int get_branch_number(const char* branch_name) {
  FILE* fbranches = fopen(".beargit/.branches", "r");

  int branch_index = -1;
  int counter = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    if (strcmp(line, branch_name) == 0) {
      branch_index = counter;
    }
    counter++;
  }

  fclose(fbranches);

  return branch_index;
}

/* beargit branch
 *
 * See "Step 5" in the project spec.
 *
 */

int beargit_branch() {
  /* COMPLETE THE REST */
  char *curr_branch = (char *)malloc(BRANCHNAME_SIZE);
  FILE* f_curr_branch = fopen(".beargit/.current_branch", "r");  
  char line[BRANCHNAME_SIZE];
  FILE* fbranches = fopen(".beargit/.branches", "r");  

  fscanf(f_curr_branch, "%s", curr_branch);  
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");

    if (!strcmp(line, curr_branch)) {
      printf("*  %s\n", line);
      continue;
    }
    printf("   %s\n", line);
  }
  free(curr_branch);
  return 0;
}

/* beargit checkout
 *
 * See "Step 6" in the project spec.
 *
 */

int checkout_commit(const char* commit_id) {
  /* COMPLETE THE REST */

  return 0;
}

int is_it_a_commit_id(const char* commit_id) {
  char *curr_dir = (char *) malloc(FILENAME_SIZE);  
  char *current_commit_id = (char *) malloc(COMMIT_ID_SIZE);  
  const char *beargit = ".beargit/";
  const char *prev = ".prev";

  strcpy(curr_dir, beargit);
  while(1) {
    char *temp_dir = (char *) malloc(FILENAME_SIZE);
    strcpy(temp_dir, curr_dir);
    strcat(temp_dir, prev);
    FILE* f_temp_dir = fopen(temp_dir, "r");
    char *current_commit_id = (char *)malloc(COMMIT_ID_SIZE);

    fscanf(f_temp_dir, "%s", current_commit_id);
    fclose(f_temp_dir);

    if (!strcmp(commit_id, current_commit_id)) {

      free(curr_dir);
      free(current_commit_id);
      free(temp_dir);
      return 1;      
    }
      
    memset(temp_dir, '\0', sizeof(temp_dir));
   
    strcpy(temp_dir, beargit);
    strcat(temp_dir, current_commit_id);
    strcat(temp_dir, "/");
    if (!fs_check_dir_exists(temp_dir)) {
      // free(curr_dir);
      // free(current_commit_id);
      // free(temp_dir);
      return 0;
    }      
    memset(curr_dir, '\0', sizeof(curr_dir));
    strcpy(curr_dir, temp_dir);
    free(temp_dir);
  }
  
  return 0;
}

int beargit_checkout(const char* arg, int new_branch) {  
  // jk: new_branch indicates whether cheate a new branch
  // jk: arg indicates the real checkout argument: a new branch name or a commit_ID
  // Get the current branch
  //fprintf(stdout, "%s\n", "in beargit_checkout 0");
  char current_branch[BRANCHNAME_SIZE];
  //fprintf(stdout, "%s\n", "in beargit_checkout 1");
  FILE* f_curr_branch = fopen(".beargit/.current_branch", "r");  
  fscanf(f_curr_branch, "%s", current_branch);  
  fclose(f_curr_branch);

  // If not detached, update the current branch by storing the current HEAD into that branch's file...
  if (strlen(current_branch)) {  // jk: not detached <=> in the HEAD of another branch
    char current_branch_file[BRANCHNAME_SIZE+50];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);  //.branch_comitID
    FILE* f_curr_branch = fopen(current_branch_file, "w");
    fprintf(f_curr_branch, "%s", "");
    fclose(f_curr_branch);      
    fs_cp(".beargit/.prev", current_branch_file);
  }

  // Check whether the argument is a commit ID. If yes, we just stay in detached mode
  // without actually having to change into any other branch.
  if (is_it_a_commit_id(arg)) {
    char commit_dir[FILENAME_SIZE] = ".beargit/";
    strcat(commit_dir, arg);  // commit dir = .beargit/commitID
    if (!fs_check_dir_exists(commit_dir)) {
      fprintf(stderr, "ERROR:  Commit %s does not exist.\n", arg);
      return 1;
    }

    // Set the current branch to none (i.e., detached).
    FILE* f_curr_branch = fopen(".beargit/.current_branch", "w");
    fprintf(f_curr_branch, "%s", "");
    fclose(f_curr_branch);    

    return checkout_commit(arg);  // jk: checkout to detached;
  }

  // Just a better name, since we now know the argument is a branch name.
  const char* branch_name = arg;

  // Read branches file (giving us the HEAD commit id for that branch).
  int branch_exists = (get_branch_number(arg) >= 0);

  // Check for errors.
  if (branch_exists && new_branch) {  // jk: change 
    fprintf(stderr, "ERROR:  A branch named %s already exists.\n", branch_name);
    return 1;
  } else if (!branch_exists && !new_branch) {
    fprintf(stderr, "ERROR: No branch %s exists\n", branch_name);
    return 1;
  }

  // File for the branch we are changing into.
  char *branch_file = (char *)malloc(BRANCHNAME_SIZE);
  strcpy(branch_file, ".beargit/.branch_");
  strcat(branch_file, branch_name);

  // Update the branch file if new branch is created (now it can't go wrong anymore)
  if (new_branch) {
    FILE* fbranches = fopen(".beargit/.branches", "w");
    fprintf(fbranches, "%s\n", branch_name);
    fclose(fbranches);
    fs_cp(".beargit/.prev", branch_file);
  }

  FILE* f_curr_branch2 = fopen(".beargit/.current_branch", "w");
  fprintf(f_curr_branch2, "%s\n", branch_name);
  fclose(f_curr_branch2);  

  // Read the head commit ID of this branch.
  char branch_head_commit_id[COMMIT_ID_SIZE];
  FILE* f_branch = fopen(branch_file, "r");
  fscanf(f_branch, "%s", branch_head_commit_id);
  fclose(f_branch);
  printf("the brnach head to commit: %s\n", branch_head_commit_id);
  // Check out the actual commit.
  return checkout_commit(branch_head_commit_id);
}

/* beargit reset
 *
 * See "Step 7" in the project spec.
 *
 */

int beargit_reset(const char* commit_id, const char* filename) {
  if (!is_it_a_commit_id(commit_id)) {
      fprintf(stderr, "ERROR:  Commit %s does not exist.\n", commit_id);
      return 1;
  }

  // Check if the file is in the commit directory
  /* COMPLETE THIS PART */

  // Copy the file to the current working directory
  /* COMPLETE THIS PART */

  // Add the file if it wasn't already there
  /* COMPLETE THIS PART */

  return 0;
}

/* beargit merge
 *
 * See "Step 8" in the project spec.
 *
 */

int beargit_merge(const char* arg) {
  // Get the commit_id or throw an error
  char commit_id[COMMIT_ID_SIZE];
  if (!is_it_a_commit_id(arg)) {
      if (get_branch_number(arg) == -1) {
            fprintf(stderr, "ERROR:  No branch or commit %s exists.\n", arg);
            return 1;
      }
      char branch_file[FILENAME_SIZE];
      snprintf(branch_file, FILENAME_SIZE, ".beargit/.branch_%s", arg);
      read_string_from_file(branch_file, commit_id, COMMIT_ID_SIZE);
  } else {
      snprintf(commit_id, COMMIT_ID_SIZE, "%s", arg);
  }

  // Iterate through each line of the commit_id index and determine how you
  // should copy the index file over
   /* COMPLETE THE REST */

  return 0;
}
