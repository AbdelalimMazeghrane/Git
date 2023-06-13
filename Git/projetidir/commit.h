#ifndef COMMIT_H
#define COMMIT_H
#include "list.h"
typedef struct key_value_pair {
  char* key;
  char* value;
} kvp;

typedef struct hash_table {
  kvp** T;
  int n;
  int size;
} HashTable;
typedef HashTable Commit;

kvp* createKeyVal(char* key, char* val);
void freeKeyVal(kvp* kv);
char* kvts(kvp* k);
kvp* stkv(char* str);
unsigned long hash(char* str);
void commitSet(Commit* c, char* key, char* value);
Commit* createCommit(char* hash);
char* commitGet(Commit* c, char* key);
char* cts(Commit* c);
Commit* stc(char* ch);
void ctf(Commit* c, char* file);
Commit* ftc(char* file);
char* blobCommit(Commit* c);
void initRefs(void);
void createUpdateRef(char* ref_name, char* hash);
void deleteRef(char* ref_name);
void myGitAdd(char* file_or_folder);
void myGitCommit(char* branch_name, char* message);
void initBranch(void);
int branchExists(char* branch);
void createBranch(char* branch);
char* getCurrentBranch(void);
void printBranch(char* branch);
List* branchList(char* branch);
List* getAllCommits(void);
void restoreCommit(char* hash_commit);
void myGitCheckoutCommit(char* pattern);
void myGitCheckoutBranch(char* branch);
List *merge(char *remote_branch, char *message);
void createDeletionCommit(char* branch, List* conflicts, char* message);
#endif
