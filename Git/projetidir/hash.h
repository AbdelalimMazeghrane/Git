#ifndef HASH_H
#define HASH_H
#include "list.h"

int hashFile(char* source, char* dest);
char* sha256file(char* file);
char* hashToPath(char* hash);
void blobFile(char* file);
char* hashToFile(char* hash);

#endif
