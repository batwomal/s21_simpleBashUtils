#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STEP 4

typedef struct {
  char **data;
  size_t length;
  size_t capacity;
  size_t step;
} vector_t;

void s21_vector_init(vector_t *vec);

void s21_vector_push_back(vector_t *vec, const char *str);

vector_t make_unique_lines(vector_t *lineList);

void s21_vector_free(vector_t *vec);
