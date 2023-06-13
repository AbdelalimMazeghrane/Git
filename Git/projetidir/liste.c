#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "list.h"

// Initialisation d'une liste vide
List *initList() {
  List *l = (List *)malloc(sizeof(List));
  *l = NULL;
  return l;
}

void libererListe(List *l) {
  Cell *L = *l;
  Cell *tmp;
  while (L != NULL) {
    tmp = L;
    free(tmp->data);
    L = L->next;
    free(tmp);
  }
  free(l);
}

// Permet d'allouer et de retourner une cellule de la liste
Cell *buildCell(char *ch) {
  Cell *c = (Cell *)malloc(sizeof(Cell));
  c->data = strdup(ch);
  c->next = NULL;
  return c;
}

// Ajout d'une cellule en tête de liste
void insertFirst(List *L, Cell *C) {
  Cell *liste = *L;
  C->next = liste;
  *L = C;
}

// Renvoie la chaîne de caractères contenue dans la cellule
char *ctos(Cell *c) { return c->data; }

// Transforme une liste en une chaîne de caractères avec le format suivant :
// chaîne1|chaîne2|chaîne3|... ex : liste contenant les chaînes "toto", "tata",
// "titi" -> "toto|tata|titi|"
char *ltos(List *L) {
  List l = *L;
  List tete = *L;
  char *chaine = (char *)malloc(256 * sizeof(char));
  while (l) {
    strcat(chaine, ctos(l));
    strcat(chaine, "|");
    l = l->next;
  }
  L = &tete;
  return chaine;
}

// Renvoie la cellule d'indice i
Cell *listGet(List *L, int i) {
  List l = *L;
  List tete = l;
  for (int j = 0; j < i; j++) {
    l = l->next;
  }
  L = &tete;
  return l;
}

// Renvoie la cellule contenant la chaîne de caractères str
Cell *searchList(List *L, char *str) {
  List l = *L;
  List tete = l;
  while (l != NULL) {
    if (strcmp(l->data, str) == 0) {
      return l;
    }
    l = l->next;
  }
  return NULL;
}

// Inverse de ltos, transforme une chaîne de caractères en liste
List *stol(char *s) {
  List *liste = initList();
  char tmp[256] = "";
  while (*s != '\0') {
    if (s[0] != '|') {
      strncat(tmp, s, 1);
      s = s + 1;
    } else {
      insertFirst(liste, buildCell(tmp));
      strcpy(tmp, "");
      s = s + 1;
    }
  }
  if (strlen(tmp) > 0) {
    insertFirst(liste, buildCell(tmp));
  }
  return liste;
}

// Ecrire une liste dans un fichier
void ltof(List *L, char *path) {
  List l = *L;
  FILE *f = fopen(path, "w");
  while (l) {
    fprintf(f, "%s\n", l->data);
    l = l->next;
  }
  fclose(f);
}

// Lire une liste depuis un fichier
List *ftol(char *path) {
  List *liste = initList();
  FILE *f = fopen(path, "r");
  char tmp[256];
  while (fgets(tmp, 256, f)) {
    insertFirst(liste, buildCell(tmp));
  }
  return liste;
}

// Liste les fichiers d'un répertoire
List *listdir(char *root_dir) {
  DIR *dir = opendir(root_dir);
  struct dirent *ep;
  List *L = initList();
  if (dir != NULL) {
    while ((ep = readdir(dir)) != NULL) {
      insertFirst(L, buildCell(ep->d_name));
    }
  }
  closedir(dir);
  return L;
}

// Renvoie 1 si le fichier existe dans le répertoire courant, 0 sinon

int file_exists(char *file) {
  List *L = listdir(".");
  List l = *L;
  List tmp = l;
  int tmp2 = 0;
  if (searchList(L, file) != NULL) {
    tmp2 = 1;
  }
  libererListe(L);
  return tmp2;
}

int file_exists_rep(char *file, char *repertoire) {
  List *L = listdir(repertoire);
  List l = *L;
  List tmp = l;
  int tmp2 = 0;
  if (searchList(L, file) != NULL) {
    tmp2 = 1;
  }
  libererListe(L);
  return tmp2;
}

// Copie le contenu du fichier from dans le fichier to
void cp(char *to, char *from) {
  FILE *fr = fopen(from, "r");
  FILE *fw = fopen(to, "w");
  char buffer[256];
  while (fgets(buffer, 256, fr)) {
    fputs(buffer, fw);
  }
  fclose(fw);
  fclose(fr);
}

List *filterlist(List *l, char *pattern) {
  List *liste = initList();
  List tmp = *l;
  List tete = tmp;
  while (tmp != NULL) {
    if (strstr(tmp->data, pattern) != NULL) {
      insertFirst(liste, buildCell(tmp->data));
    }
    tmp = tmp->next;
  }
  l = &tete;
  return liste;
}

int nbElements(List *L) {
  int res = 0;
  Cell *c = *L;
  while (c != NULL) {
    res++;
    c = c->next;
  }
  return res;
}