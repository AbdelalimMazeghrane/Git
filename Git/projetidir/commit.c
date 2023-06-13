#include "commit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hash.h"
#include "list.h"
#include "work.h"
#define SIZECOMMIT 30

// crée un nouveau key value pair
kvp *createKeyVal(char *key, char *val) {
  kvp *k = malloc(sizeof(kvp));
  k->key = strdup(key);
  k->value = strdup(val);
  return k;
}

// libère la mémoire allouée pour un key value pair
void freeKeyVal(kvp *kv) {
  free(kv->key);
  free(kv->value);
  free(kv);
}

// affiche un key value pair sous la forme "key : value"
char *kvts(kvp *k) {
  char *buff = malloc(sizeof(char) * 256);
  sprintf(buff, "%s : %s", k->key, k->value);
  return buff;
}

// crée un key value pair à partir d'une chaîne de caractères
kvp *stkv(char *str) {
  char key[100], val[100];
  sscanf(str, "%s : %s", key, val);
  return createKeyVal(key, val);
}

// crée un nouveau commit et l'initialise
Commit *initCommit() {
  Commit *c = malloc(sizeof(Commit));
  c->T = malloc(SIZECOMMIT * sizeof(kvp *));
  c->size = SIZECOMMIT;
  for (int i = 0; i < c->size; i++) {
    c->T[i] = NULL;
  }
  c->n = 0;
  return c;
}

// fonction de hashage
unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++) != '\0')
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

// set un key value pair dans un commit
void commitSet(Commit *c, char *key, char *value) {
  int p = hash(key) % c->size;
  while (c->T[p] != NULL) {
    p = (p + 1) % c->size;  // probing lineaire
  }
  c->T[p] = createKeyVal(key, value);
  c->n++;
}

// crée un commit à partir d'un hash
Commit *createCommit(char *hash) {
  Commit *c = initCommit();
  commitSet(c, "tree", hash);
  return c;
}

// Récupere la valeur associée à une clé dans un commit
char *commitGet(Commit *c, char *key) {
  int p = hash(key) % c->size;
  int attempt = 0;
  while (c->T[p] != NULL && attempt < c->size) {
    if (strcmp(c->T[p]->key, key) == 0) {
      return c->T[p]->value;
    }
    p = (p + 1) % c->size;
    attempt = attempt + 1;
  }
  return NULL;
}

// convertit un commit en chaîne de caractères
char *cts(Commit *c) {
  char *str = malloc(sizeof(char) * 100 * c->n);
  for (int i = 0; i < c->size; i++) {
    if (c->T[i] != NULL) {
      strcat(str, kvts(c->T[i]));
      strcat(str, "\n");
    }
  }
  return str;
}

//convertit une chaîne de caractères en commit
Commit *stc(char *ch) {
  int pos = 0;
  int n_pos = 0;
  char sep = '\n';
  char *ptr;
  char *result = malloc(sizeof(char) * 10000);
  Commit *c = initCommit();
  while (pos < strlen(ch)) {
    ptr = strchr(ch + pos, sep);
    if (ptr == NULL)
      n_pos = strlen(ch) + 1;
    else
      n_pos = ptr - ch + 1;
    memcpy(result, ch + pos, n_pos - pos - 1);
    result[n_pos - pos - 1] = '\0';
    pos = n_pos;
    kvp *elem = stkv(result);
    commitSet(c, elem->key, elem->value);
  }
  return c;
}

// convertit un commit en fichier
void ctf(Commit *c, char *file) {
  FILE *fp = fopen(file, "w");
  if (fp != NULL) {
    fputs(cts(c), fp);
    fclose(fp);
  }
}

// convertit un fichier en commit
Commit *ftc(char *file) {
  char buff[256];
  char *all = malloc(sizeof(char) * 256);
  FILE *f = fopen(file, "r");
  if (f == NULL) {
    printf("fichier inexistant %s \n",file);
    return NULL;
  }
  while (fgets(buff, 256, f) != NULL) {
    strcat(all, buff);
  }
  Commit *c = stc(all);
  return c;
}

// fais l'enregistrement d'un commit
char *blobCommit(Commit *c) {
  char fname[100] = "/tmp/myfileXXXXXX";
  int fd = mkstemp(fname);
  ctf(c, fname);
  char *hash = sha256file(fname);
  char *ch = hashToFile(hash);
  strcat(ch, ".c");
  cp(ch, fname);
  return hash;
}

// initialise les références
void initRefs() {
  if (!file_exists(".refs")) {
    system("mkdir .refs");
    system("touch .refs/master ");
    system("touch .refs/HEAD");
  }
}

// crée une référence
void createUpdateRef(char *ref_name, char *hash) {
  char buff[100];
  sprintf(buff, "echo %s > .refs/%s", hash, ref_name);
  system(buff);
}

// suppression d'une référence
void deleteRef(char *ref_name) {
  char buff[256];
  if (!file_exists_rep(ref_name, ".refs/")) {
    printf("La référence %s n'existe pas\n", ref_name);
  } else {
    sprintf(buff, "rm .refs/%s", ref_name);
    system(buff);
  }
}


// récupère le hash d'une référence
char *getRef(char *ref_name) {
  int SIZE = 65;
  FILE *fp;
  char *result = malloc(sizeof(char) * SIZE);

  if (!file_exists_rep(ref_name, ".refs/")) {
    printf("La référence %s n'existe pas\n", ref_name);
    return NULL;
  }
  char buff[256];
  sprintf(buff, ".refs/%s", ref_name);
  fp = fopen(buff, "r");
  if (fp == NULL) {
    printf("Erreur à l'ouverture du fichier  \n");
    return NULL;
  }
  fgets(result, SIZE, fp);
  fclose(fp);
  return result;
}

void myGitAdd(char *file_or_folder) {
  WorkTree *wt;
  if (!file_exists(".add")) {
    system("touch .add");
    wt = initWorkTree();
  } else {
    wt = ftwt(".add");
    // printf("%s\n", wtts(wt));
  }
  if (file_exists(file_or_folder)) {
    appendWorkTree(wt, file_or_folder, NULL, 0);
    wttf(wt, ".add");
  } else {
    printf("Le fichier/repertoire %s n'existe pas\n", file_or_folder);
  }
}


void myGitCommit(char *branch_name, char *message) {
  if (!file_exists(".refs")) {
    printf("Il faut d’abord initialiser les références du projet\n");
    return;
  }
  if (!file_exists_rep(branch_name, ".refs/")) {
    printf("La branche %s n’ existe pas\n", branch_name);
    return;
  }
  char *last_hash = getRef(branch_name);
  char *head_hash = getRef("HEAD");
  if (strcmp(last_hash, head_hash) != 0) {
    printf("HEAD doit pointer sur le dernier commit de la branche\n");
    return;
  }
  WorkTree *wt = ftwt(".add");
  char *hashwt = saveWorkTree(wt, ".");
  Commit *c = createCommit(hashwt);
  if (strlen(last_hash) != 0) {
    commitSet(c, "predecessor", last_hash);
  }
  if (message != NULL) {
    commitSet(c, "message", message);
  }
  char *hashc = blobCommit(c);
  createUpdateRef(branch_name, hashc);
  createUpdateRef("HEAD", hashc);
  system("rm .add ");
}

// Fonction void initBranch() qui crée le fichier caché .current branch
// contenant la chaîne de caractères "master"
void initBranch() {
  FILE *fp = fopen(".current_branch", "w");
  if (fp != NULL) {
    fputs("master", fp);
    fclose(fp);
  }
}

// Fonction int branchExists(char* branch) qui vérifie l’existence d'une branche
int branchExists(char *branch) {
  List *refs = listdir(".refs");
  return searchList(refs, branch) != NULL;
}

// Fonction void createBranch(char* branch) qui crée une branche qui pointe vers
// le même commit que HEAD
void createBranch(char *branch) {
  char *hash = getRef("HEAD");
  createUpdateRef(branch, hash);
}

// Fonction char* getCurrentBranch() qui lit le fichier caché .current branch
// pour retourner le nom de la branche courante
char *getCurrentBranch() {
  FILE *f = fopen(".current_branch", "r");
  char *buff = malloc(sizeof(char) * 100);
  fscanf(f, "%s", buff);
  return buff;
}

void printBranch(char *branch) {
  char *commit_path = malloc(sizeof(char) * 100);
  char *commit_hash = getRef(branch);
  sprintf(commit_path, "%s.c", hashToPath(commit_hash));
  Commit *c = ftc(commit_path);
  while (c != NULL) {
    if (commitGet(c, "message") != NULL)
      printf("%s -> %s\n", commit_hash, commitGet(c, "message"));
    else {
      printf("%s\n", commit_hash);
    }
    if (commitGet(c, "predecessor") != NULL) {
      commit_path = NULL;
      commit_hash = commitGet(c, "predecessor");
      sprintf(commit_path, "%s.c", hashToPath(commit_hash));
      c = ftc(commit_path);
    } else {
      c = NULL;
      free(commit_path);
      free(commit_hash);
    }
  }
}

List *branchList(char *branch) {
  List *l = initList();
  char *commit_hash = getRef(branch);
  char *commit_path = malloc(sizeof(char) * 68);
  sprintf(commit_path, "%s.c", hashToPath(commit_hash));
  Commit *c = ftc(commit_path);
  while (c != NULL) {
    insertFirst(l, buildCell(commit_hash));
    if (commitGet(c, "predecessor") != NULL) {
      strcpy(commit_path,"");
      commit_hash = commitGet(c, "predecessor");
      sprintf(commit_path, "%s.c", hashToPath(commit_hash));
      c = ftc(commit_path);
    } else {
      c = NULL;
      free(commit_path);
      free(commit_hash);
    }
  }
  return l;
}

List *getAllCommits() {
  List *listCommits = initList();
  List *listBranchName = listdir(".refs");
  for (Cell *branchName = *listBranchName; branchName != NULL;
       branchName = branchName->next) {
        if(strcmp(branchName->data,".")==0 || strcmp(branchName->data,"..")==0){
          continue;
        }
    List *tmpList = branchList(branchName->data);
    Cell *cell = *tmpList;
    while (cell != NULL) {
      if (searchList(listCommits, cell->data) != NULL) {
        insertFirst(listCommits, buildCell(cell->data));
      }
      cell = cell->next;
    }
    libererListe(tmpList);
  }
  libererListe(listBranchName);
  return listCommits;
}

// Permet de restaurer le worktree associé à un commit dont le hash est donné en
// paramètre. Indication : Il faut utiliser la fonction restoreWorkTree
void restoreCommit(char *hash_commit) {
  char *commit_path = malloc(sizeof(char) * 68);
  sprintf(commit_path, "%s.c", hashToPath(hash_commit));
  Commit *c = ftc(commit_path);
  char *hashwt = commitGet(c, "tree");
  char *path = hashToPath(hashwt);
  sprintf(path, "%s.t", path);
  WorkTree *wt = ftwt(path);
  restoreWorkTree(wt, ".");
}

void myGitCheckoutCommit(char *pattern) {
  // On récupère la liste de tous les commits existants.
  List *listCommits = getAllCommits();
  // On filtre cette liste pour ne garder que ceux qui commencent par pattern.
  List *List = filterlist(listCommits, pattern);
  libererListe(listCommits);
  int nbHash = nbElements(List);
  if (nbHash == 0) {
    // S'il ne reste plus aucun hash après le filtre, la fonction affiche un
    // message d'erreur à l'utilisateur.
    printf("Aucun commit ne correspond à votre requête.\n");
  } else if (nbHash == 1) {
    // S'il ne reste plus qu’un hash après ce filtre, alors on met à jour la
    // référence HEAD pour pointer sur ce hash, et on restaure le worktree
    // correspondant.
    char *hash = (*List)->data;
    createUpdateRef("HEAD", hash);
    restoreCommit(hash);
  } else {
    // S'il reste plusieurs hash après le filtre, la fonction les affiche tous
    // et demande à l'utilisateur de préciser sa requête.
    printf("Plusieurs commits correspondent à votre requête :\n");
    for (Cell *cell = *List; cell != NULL; cell = cell->next) {
      printf("%s\n", cell->data);
    }
    printf("Veuillez préciser votre requête.\n");
  }
}

void myGitCheckoutBranch(char *branch) {
  // On modifie le fichier .current branch pour contenir le nom de la branche
  // donnée en paramètre
  FILE *f = fopen(".current_branch", "w");
  if (f != NULL) {
    fputs(branch, f);
    fclose(f);
  }
  // On modifie la référence HEAD pour contenir le hash du dernier commit de
  // branch (on rappelle que ce hash est contenu dans le fichier branch).
  char *hash = getRef(branch);
  createUpdateRef("HEAD", hash);
  // On restaure le worktree correspondant au dernier commit de branch.
  restoreCommit(hash);
}

List *merge(char *remote_branch, char *message){
  // On récupère le nom de la branche courante
  char *current_branch = getCurrentBranch();
  // On récupère le hash du dernier commit de la branche courante et de la branche en paramètre
  char *hash_current_branch = getRef(current_branch);
  char *hash_remote_branch = getRef(remote_branch);
  // On récupère les chemins des fichiers contenant les commits correspondant aux deux hash
  char *path_current_branch = hashToPath(hash_current_branch);
  char *path_remote_branch = hashToPath(hash_remote_branch);
  // On ajoute l'extension .c aux chemins
  sprintf(path_current_branch, "%s.c", path_current_branch);
  sprintf(path_remote_branch, "%s.c", path_remote_branch);
  // On récupère les commits correspondants aux deux hash
  Commit *commit_current_branch = ftc(path_current_branch);
  Commit *commit_remote_branch = ftc(path_remote_branch);
  // On récupère les chemins des fichiers contenant les worktrees correspondant aux deux commits
  char *hash_current_branch_worktree = commitGet(commit_current_branch, "tree");
  char *hash_remote_branch_worktree = commitGet(commit_remote_branch, "tree");
  // On récupère les chemins des fichiers contenant les worktrees correspondant aux deux commits
  char *path_current_branch_worktree = hashToPath(hash_current_branch_worktree);
  char *path_remote_branch_worktree = hashToPath(hash_remote_branch_worktree);
  // On ajoute l'extension .t aux chemins
  sprintf(path_current_branch_worktree, "%s.t", path_current_branch_worktree);
  sprintf(path_remote_branch_worktree, "%s.t", path_remote_branch_worktree);
  // On récupère les worktrees correspondants aux deux commits
  WorkTree *wt_current_branch = ftwt(path_current_branch_worktree);
  WorkTree *wt_remote_branch = ftwt(path_remote_branch_worktree);
  List *list_conflicts = initList();
  // On fusionne les deux worktrees
  WorkTree *mergeWT = mergeWorkTrees(wt_current_branch, wt_remote_branch, &list_conflicts);
  if(list_conflicts == NULL){ 
    // — créer le commit associé à ce nouveau worktree, en indiquant qui sont ses prédécesseurs, et en lui ajoutant le message descriptif passé en paramètre,
    char *hash_mergeWT = blobWorkTree(mergeWT);
    Commit *new_commit = createCommit(hash_mergeWT);
    commitSet(new_commit, "message", message);
    commitSet(new_commit, "predecessor", hash_current_branch);
    commitSet(new_commit, "merged_predecessor", hash_remote_branch);
    // — réaliser un enregistrement instantané du worktree de fusion et de ce nouveau commit,
    char *path = hashToPath(hash_mergeWT);
    sprintf(path, "%s.t", path);
    hash_mergeWT = saveWorkTree(mergeWT, path);
    char *hash_new_commit = blobCommit(new_commit);
    //ajouter le nouveau commit à la branche courante
    //mettre à jour la référence de la branche courante et la référence HEAD pour pointer vers ce nouveau commit,
    createUpdateRef(current_branch, hash_new_commit);
    createUpdateRef("HEAD", hash_new_commit);
    //supprimer la référence de la branche passée en paramètre,
    deleteRef(remote_branch);
    //restaurer le projet correspondant au worktree de fusion.
    restoreWorkTree(mergeWT, ".");
    return NULL;
  }
  return list_conflicts;
}

/*On souhaite maintenant pouvoir créer des commits de suppression en cas de conflits. Ecrivez ´
une fonction void createDeletionCommit(char* branch, List* conflicts, char* message) qui
crée et ajoute un commit de suppression sur la branche branch, correspondant à la suppression des
éléments de la liste conflicts. Pour ce faire, on peut procéder ainsi :
— On commence par se déplacer sur la branche en question (à l’aide de myGitCheckoutBranch).
— On récupère le dernier commit de cette branche, et le worktree associ´e,
— On vide la zone de préparation (c-à-d le fichier “.add”), puis on utilise la fonction myGitAdd
pour ajouter les fichiers/répertoires du worktree qui ne font pas partie de la liste des conflits.
— On appelle la fonction myGitCommit pour cr´eer le commit de suppression.
— On revient sur la branche de d´epart (à l’aide de myGitCheckoutBranch).*/

void createDeletionCommit(char* branch, List* conflicts, char* message){
  //On récupère le nom de la branche courante
  char *start_branch = getCurrentBranch();
  //On se déplace sur la branche en question
  myGitCheckoutBranch(branch);
  //On récupère le dernier commit de cette branche, et le worktree associé
  char *hash_current_branch = getRef(branch);
  char *path_current_branch = hashToPath(hash_current_branch);
  sprintf(path_current_branch, "%s.c", path_current_branch);
  Commit *commit_current_branch = ftc(path_current_branch);
  char *hash_current_branch_worktree = commitGet(commit_current_branch, "tree");
  char *path_current_branch_worktree = hashToPath(hash_current_branch_worktree);
  sprintf(path_current_branch_worktree, "%s.t", path_current_branch_worktree);
  WorkTree *wt_current_branch = ftwt(path_current_branch_worktree);
  //On vide la zone de préparation
  system("rm .add");
  //On ajoute les fichiers/répertoires du worktree qui ne font pas partie de la liste des conflits
  for(int i = 0; i < wt_current_branch->n; i++){
    if(!searchList(conflicts, wt_current_branch->tab[i].name)){
      myGitAdd(wt_current_branch->tab[i].name);
    }
  }
  //On appelle la fonction myGitCommit pour créer le commit de suppression
  myGitCommit(branch,message);
  //On revient sur la branche de départ
  myGitCheckoutBranch(start_branch);
}