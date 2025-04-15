#ifndef COMMN_H
#define COMMN_H

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void s21_error(char *pname, char *message);
void *s21_calloc(size_t size);
void *s21_realloc(void *memblock, size_t size);

char *s21_strdup(const char *str);

#endif
