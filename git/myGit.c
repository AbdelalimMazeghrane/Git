#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "commit.h"
#include "hash.h"
#include "list.h"
#include "work.h"

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    printf("Erreur : ./myGit <command>\n");
    return 1;
  }
  if (strcmp(argv[1], "init") == 0) {
    initRefs();
    initBranch();
  } else if (strcmp(argv[1], "list-refs") == 0) {
    printf("REFS : \n");
    if (file_exists(".refs")) {
      List* L = listdir(".refs");
      for (Cell* ptr = *L; ptr != NULL; ptr = ptr->next) {
        if (ptr->data[0] == '.') continue;
        printf("%s\n", ptr->data);
      }
    }
  }

  else if (strcmp(argv[1], "create-ref") == 0) {
    if (argc < 4) {
      printf("Erreur : ./myGit create-ref <ref-name> <hash>\n");
      return 1;
    }
    createUpdateRef(argv[2], argv[3]);

  }

  else if (strcmp(argv[1], "delete-ref") == 0) {
    if (argc < 3) {
      printf("Erreur : ./myGit delete-ref <ref-name>\n");
      return 1;
    }
    deleteRef(argv[2]);
  }

  else if (strcmp(argv[1], "add") == 0) {
    for (int i = 2; i < argc; i++) {
      myGitAdd(argv[i]);
    }
  }

  else if (strcmp(argv[1], "clear-add") == 0) {
    system("rm .add ");
  } else if (strcmp(argv[1], "list-add") == 0) {
    printf("Zone de préparation : \n");
    if (file_exists(".add")) {
      WorkTree* wt = ftwt(".add");
      printf("%s \n", wtts(wt));
    }
  }

  else if (strcmp(argv[1], "commit") == 0) {
    if (argc > 3) {
      if (strcmp(argv[3], "-m") == 0) {
        myGitCommit(argv[2], argv[4]);
      }
    } else {
      if (argc < 3) {
        printf("Erreur : ./myGit commit <ref-name>\n");
        return 1;
      }
      myGitCommit(argv[2], NULL);
    }
  } else if (strcmp(argv[1], "get-current-branch") == 0) {
    printf("%s\n", getCurrentBranch());
  } else if (strcmp(argv[1], "branch") == 0) {
    if (argc < 3) {
      printf("Erreur : ./myGit branch <branch-name>\n");
      return 1;
    }
    if (branchExists(argv[2])) {
      printf("Erreur : la branche %s existe déja \n", argv[2]);
      return 1;
    }
    createBranch(argv[2]);
  } else if (strcmp(argv[1], "branch-print") == 0) {
    if (argc < 3) {
      printf("Erreur : ./myGit branch-print <branch-name>\n");
      return 1;
    }
    printBranch(argv[2]);
  }

  else if (strcmp(argv[1], "checkout-branch") == 0) {
    if (argc < 3) {
      printf("Erreur : ./myGit checkout-branch <branch-name>\n");
      return 1;
    }
    myGitCheckoutBranch(argv[2]);
  }

  else if (strcmp(argv[1], "checkout-commit") == 0) {
    if (argc < 3) {
      printf("Erreur : ./myGit checkout-commit <pattern>\n");
      return 1;
    }
    myGitCheckoutCommit(argv[2]);
  }
  
  else if(strcmp(argv[1], "merge") == 0){
    if (argc < 4) {
      printf("Erreur : ./myGit merge <branch> <message>\n");
      return 1;
    }
    List *conflicts = merge(argv[2], argv[3]);
    if(conflicts == NULL){
      printf("Merge réussi\n");
    }
    else{
      printf("Il y a des conflits, veuillez faire un choix parmis les suivants (1/2/3) : \n");
      printf("1. Garder les fichiers de la branche courante, \n");
      printf("2. Garder les fichiers de la branche <branch\n");
      printf("3. Choisir manuellement, conflit par conflit\n");
      int choix;
      scanf("%d", &choix);
      if(choix == 1){
        createDeletionCommit(argv[2], conflicts,"");
        merge(argv[2],argv[3]);
      }
      else if(choix == 2){
        createDeletionCommit(getCurrentBranch(), conflicts,"");
        merge(argv[2], argv[3]);
      }
      else if(choix == 3){
        List *conflicts1 = NULL;
        List *conflicts2 = NULL;
        for(Cell *ptr = *conflicts; ptr != NULL; ptr = ptr->next){
          printf("Voulez vous garder le fichier %s de la branche courante ? (1/0)\n", ptr->data);
          int choix;
          scanf("%d", &choix);
          if(choix == 1){
            insertFirst(conflicts1, buildCell(ptr->data));
          }
          else{
            insertFirst(conflicts2, buildCell(ptr->data));
          }
        }
        createDeletionCommit(getCurrentBranch(), conflicts1,"");
        createDeletionCommit(argv[2], conflicts2,"");
        merge(argv[2], argv[3]);
        libererListe(conflicts1);
        libererListe(conflicts2);
      }
    }
    libererListe(conflicts);
  }
  return 0;
}
