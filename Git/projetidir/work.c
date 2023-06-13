#include "work.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hash.h"
#include "list.h"
#define SIZETREE 10

// Fonction donnée dans le sujet
// Permet de récupérer le mode d'un fichier
int getChmod(const char* path) {
  struct stat ret;
  if (stat(path, &ret) == -1) {
    return -1;
  }
  return (ret.st_mode & S_IRUSR) | (ret.st_mode & S_IWUSR) |
         (ret.st_mode & S_IXUSR) | /* owner*/
         (ret.st_mode & S_IRGRP) | (ret.st_mode & S_IWGRP) |
         (ret.st_mode & S_IXGRP) | /* group*/
         (ret.st_mode & S_IROTH) | (ret.st_mode & S_IWOTH) |
         (ret.st_mode & S_IXOTH); /* other*/
}

// Fonction donnée dans le sujet
// Permet de modifier le mode d'un fichier
void setMode(int mode, char* path) {
  char buff[100];
  sprintf(buff, "chmod %d %s", mode, path);
  system(buff);
}

// Crée un WorkFile et l'initialise
WorkFile* createWorkFile(char* name) {
  WorkFile* w = (WorkFile*)malloc(sizeof(WorkFile));
  w->name = strdup(name);
  w->hash = NULL;
  w->mode = 0;
  return w;
}

// WorkFile to String
// Affiche un WorkFile sous la forme "name hash mode"
char* wfts(WorkFile* wf) {
  char* s = malloc(256 * sizeof(char));
  sprintf(s, "%s\t%s\t%d", wf->name, wf->hash, wf->mode);
  return s;
}

// String to WorkFile
// Crée un WorkFile à partir d'une chaîne de caractères
WorkFile* stwf(char* ch) {
  char* name = malloc(sizeof(char) * 1000);
  char* hash = malloc(sizeof(char) * 1000);
  int mode;
  sscanf(ch, "%s\t%s\t%d", name, hash, &mode);
  WorkFile* wf = createWorkFile(name);
  wf->mode = mode;
  wf->hash = hash;
  return wf;
}

// Initialise un WorkTree et le retourne
WorkTree* initWorkTree() {
  WorkTree* wt = (WorkTree*)malloc(sizeof(WorkTree));
  wt->size = SIZETREE;
  wt->n = 0;
  wt->tab = (WorkFile*)malloc(SIZETREE * sizeof(WorkFile));
  return wt;
}

// Vérifie la présence d'un fichier ou d'un répertoire dans un WorkTree
// Retourne l'indice du fichier dans le tableau si il est présent et -1 sinon
int inWorkTree(WorkTree* wt, char* name) {
  for (int i = 0; i < wt->n; i++) {
    if (strcmp(wt->tab[i].name, name) == 0) {
      return i;
    }
  }
  return -1;
}

// Ajoute un fichier ou un répertoire dans un WorkTree
int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode) {
  int i = inWorkTree(wt, name);
  if (i != -1) {
    return 0;
  }
  if (wt->n >= wt->size) {
    printf("L'arbre est plein, impossible d'ajouter un autre fichier\n");
    return -1;
  }
  WorkFile* w = createWorkFile(name);
  w->hash = hash;
  w->mode = mode;
  wt->tab[wt->n] = *w;
  wt->n += 1;
  return 0;
}

// WorkTree to String
// Affiche un WorkTree sous la forme "name hash mode \n name hash mode \n ..."
char* wtts(WorkTree* wt) {
  char* s = malloc(10000 * sizeof(char));
  for (int i = 0; i < wt->n; i++) {
    strcat(s, wfts(&wt->tab[i]));
    strcat(s, "\n");
  }
  return s;
}

// String to WorkTree
// Crée un WorkTree à partir d'une chaîne de caractères
WorkTree* stwt(char* ch) {
  int pos = 0;
  int n_pos = 0;
  int sep = '\n';
  char* ptr;
  char* result = malloc(sizeof(char) * 10000);
  WorkTree* wt = initWorkTree();
  while (pos < strlen(ch)) {
    ptr = strchr(ch + pos, sep);
    if (ptr == NULL)
      n_pos = strlen(ch) + 1;
    else {
      n_pos = ptr - ch + 1;
    }
    memcpy(result, ch + pos, n_pos - pos - 1);
    result[n_pos - pos - 1] = '\0';
    pos = n_pos;
    WorkFile* wf = stwf(result);
    appendWorkTree(wt, wf->name, wf->hash, wf->mode);
  }
  return wt;
}

// WorkTree to File
// Ecrit un WorkTree dans un fichier
int wttf(WorkTree* wt, char* path) {
  FILE* fp = fopen(path, "w");
  if (fp != NULL) {
    fputs(wtts(wt), fp);
    fclose(fp);
    return 0;
  }
  return -1;
}

// File to WorkTree
// Crée un WorkTree à partir d'un fichier
WorkTree* ftwt(char* file) {
  FILE* fp = fopen(file, "r");
  if (fp == NULL) {
    return NULL;
  }
  WorkTree* wt = initWorkTree();
  char buffer[256];
  while (fgets(buffer, 256, fp) != NULL) {
    WorkFile* wf = stwf(buffer);
    appendWorkTree(wt, wf->name, wf->hash, wf->mode);
  }
  fclose(fp);
  return wt;
}

// Crée un fichier temporaire représentant le WorkTree pour pouvoir ensuite
// créer l’enregistrement instantané du WorkTree (avec l’extension ".t").

char* blobWorkTree(WorkTree* wt) {
  static char template[] = "/tmp/testFileXXXXX";
  char fname[1000];
  strcpy(fname, template);
  int fd = mkstemp(fname);
  wttf(wt, fname);
  char* hash = sha256file(fname);
  char* ch = hashToFile(hash);
  strcat(ch, ".t");
  cp(ch, fname);
  char command[256];
  sprintf(command, "rm %s", fname);
  system(command);
  return hash;
}

// crée un enregistrement instantané du WorkTree
char* saveWorkTree(WorkTree* wt, char* path) {
  // Parcourt le tableau de WorkFile de wt
  for (int i = 0; i < wt->n; i++) {
    WorkFile* wf = &(wt->tab[i]);
    char* absPath = malloc(strlen(path) + strlen(wf->name) + 2);
    strcpy(absPath, path);
    strcpy(absPath, "/");
    strcpy(absPath, wf->name);
    if (file_exists(absPath)) {
      // Cas où WF correspond à un fichier
      blobFile(wf->name);
      char* hash = sha256file(wf->name);
      int mode = getChmod(wf->name);
      wf->hash = hash;
      wf->mode = mode;
    } else {
      // Cas où WF correspond à un répertoire
      List* files = listdir(wf->name);
      WorkTree* newWT = initWorkTree();
      for (Cell* curr = *files; curr != NULL; curr = curr->next) {
        char* filename = curr->data;
        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
          // Ignore les répertoires . et ..
          continue;
        }
        appendWorkTree(newWT, filename, NULL, 0);
      }
      libererListe(files);
      char* newHash = saveWorkTree(newWT, absPath);
      int mode = getChmod(absPath);
      wf->hash = newHash;
      wf->mode = mode;
    }
  }
  // Enregistrement instantané de wt et retour du hash
  return blobWorkTree(wt);
}

int isWorkTree(char *hash){
    char *directory_name = strtok(hashToPath(hash), "/");
    char *file_name = strtok(NULL, "/");
    int result = file_exists_rep(file_name, directory_name);

    if(result==1){ return 0;}
    else{
      strcat(file_name,".t");
      int result = file_exists_rep(file_name, directory_name);
    }
    if(result)
      return 1;
    return -1;
}

// Restaure un WorkTree à partir d'un fichier
void restoreWorkTree(WorkTree* wt, char* path) {
  int i;
  char* copyPath;
  char* absPath;
  WorkTree* sub_wt;
  for (i = 0; i < wt->n; i++) {
    WorkFile* wf = &(wt->tab[i]);
    copyPath = hashToPath(wf->hash);
    absPath =
        (char*)malloc(sizeof(char) * (strlen(path) + strlen(wf->name) + 2));
    char* hash = wf->hash;

    sprintf(absPath, "%s/%s", path, wf->name);
    // printf("%s\n %s\n", absPath, copyPath);
    if (isWorkTree(hash) == 0) {  // C'est un fichier
      cp(absPath, copyPath);
      setMode(getChmod(copyPath), absPath);
    } else {
      if (isWorkTree(hash) == 1) {  // C'est un repertoire
        strcat(copyPath, ".t");
        sub_wt = ftwt(copyPath);
        restoreWorkTree(sub_wt, absPath);
        setMode(getChmod(copyPath), absPath);
      }
    }
  }
}

WorkTree *mergeWorkTrees(WorkTree *wt1, WorkTree *wt2, List **conflicts){
  WorkTree *result = initWorkTree();
  //on parcourt les deux worktrees
  for(int i = 0; i < wt1->n; i++){
    for(int j = 0; j < wt2->n; j++){
      //si les deux worktrees ont le meme nom
      if(strcmp(wt1->tab[i].name, wt2->tab[j].name) == 0){
        //si les deux worktrees ont le meme hash
        if(strcmp(wt1->tab[i].hash, wt2->tab[j].hash) == 0){
          //on ajoute le fichier dans le nouveau worktree
          appendWorkTree(result, wt1->tab[i].name, wt1->tab[i].hash, wt1->tab[i].mode);
        }
        else{
          //on ajoute le fichier dans la liste des conflits
          insertFirst(*conflicts, buildCell(wt1->tab[i].name));
        }
      }
    }
  }
  return result;
}