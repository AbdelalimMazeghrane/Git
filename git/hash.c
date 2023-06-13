#include "hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Calcule le hash du contenu du premier fichier et l’écrit dans le deuxième
// fichier ex: hashFile("hash.c", "hash.txt") -> hash.txt contient le hash de
// hash.c
int hashFile(char* source, char* dest) {
  char command[256];
  sprintf(command, "sha256sum %s > %s", source, dest);
  system(command);

  return 0;
}

// Renvoie une chaîne de caractères contenant le hash du fichier donné en
// paramètre ex: hash.c ->
// 1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef
char* sha256file(char* file) {
  static char template[] = "/tmp/tmpFileXXXXX";
  char fname[1000];
  strcpy(fname, template);
  int fd = mkstemp(fname);
  if (fd < 0) {
    printf("Erreur a la creation du fichier temporaire");
    exit(1);
  }
  hashFile(file, fname);
  char* valeurHash = malloc(65 * sizeof(char));
  FILE* f = fopen(fname, "r");
  fgets(valeurHash, 65, f);
  fclose(f);
  char command[256];
  sprintf(command, "rm %s", fname);
  system(command);
  return valeurHash;
}

char* hashToFile(char* hash) {
  char* ch2 = strdup(hash);
  ch2[2] = '\0';
  mkdir(ch2, 0700);
  return hashToPath(hash);
}

// Renvoie une chaîne de caractères contenant le chemin du fichier correspondant
// au hash donné en paramètre ex:
// 1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef ->
// 12/34567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef
char* hashToPath(char* hash) {
  char* path = malloc(66 * sizeof(char));
  strncat(path, hash, 2);
  strcat(path, "/");
  strcat(path, hash + 2);
  return path;
}

// Fais un enregistrement instantané du fichier donné en paramètre
void blobFile(char* file) {
  char* hash = sha256file(file);
  char* path = hashToPath(hash);
  char dir[3];
  strncpy(dir, path, 2);
  dir[2] = '\0';
  free(hash);
  char command[256];
  sprintf(command, "mkdir %s", dir);
  system(command);
  hashFile(file, path);
}