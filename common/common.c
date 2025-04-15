#include "common.h"

void s21_error(char *pname, char *message) {
  fprintf(stderr, "%s: %s\n", pname, message);
}

void *s21_calloc(size_t size) {
  void *result;
  errno = 0;
  result = malloc(size);
  if (!result) {
    s21_error("", strerror(errno));
    exit(1);
  } else {
    memset(result, 0, size);
  }
  return result;
}

void *s21_realloc(void *memblock, size_t size) {
  void *result;
  errno = 0;
  result = realloc(memblock, size);
  if (!result) {
    free(memblock);
    s21_error("", strerror(errno));
    exit(1);
  }
  return result;
}

char *s21_strdup(const char *str) {
  if (str == NULL) return NULL;
  size_t len = strlen(str) + 1;
  char *copy = s21_calloc(len);
  if (copy) {
    memcpy(copy, str, len);
  }
  return copy;
}
