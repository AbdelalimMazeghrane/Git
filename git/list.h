#ifndef LIST_H
#define LIST_H

typedef struct cell {
  char *data;
  struct cell *next;
} Cell;
typedef Cell *List;

List *initList(void);
void libererListe(List *l);
Cell *buildCell(char *ch);
void insertFirst(List *L, Cell *C);
char *ctos(Cell *c);
Cell *listGet(List *L, int i);
Cell *searchList(List *L, char *str);
List *stol(char *s);
void ltof(List *L, char *path);
List *ftol(char *path);
List *listdir(char *root_dir);
int file_exists(char *file);
int file_exists_rep(char *file, char *repertoire);
void cp(char *to, char *from);
List *filterlist(List *l, char *pattern);
int nbElements(List *l);
#endif
