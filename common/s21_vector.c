#include "s21_vector.h"

#include "common.h"

void s21_vector_init(vector_t *vec) {
  vec->data = NULL;
  vec->length = 0;
  vec->capacity = 0;
  vec->step = STEP;
}

void s21_vector_push_back(vector_t *vec, const char *str) {
  if (vec->length == vec->capacity) {
    vec->capacity += vec->step;
    vec->data = s21_realloc(vec->data, vec->capacity * sizeof(char *));
    if (!vec->data) {
      perror("Failed to reallocate memory");
      exit(EXIT_FAILURE);
    }
  }

  vec->data[vec->length] = s21_strdup(str);
  if (!vec->data[vec->length]) {
    perror("Failed to allocate memory for string");
    exit(EXIT_FAILURE);
  }
  vec->length++;
}

vector_t make_unique_lines(vector_t *lineList) {
  vector_t uniqueLines;
  s21_vector_init(&uniqueLines);
  if (lineList->data) {
    s21_vector_push_back(&uniqueLines, lineList->data[0]);
    for (size_t i = 1; i < lineList->length; i++) {
      int isUnique = 1;
      for (size_t j = 0; j < uniqueLines.length; j++) {
        if (!strcmp(uniqueLines.data[j], lineList->data[i])) isUnique = 0;
        break;
      }
      if (isUnique) s21_vector_push_back(&uniqueLines, lineList->data[i]);
    }
  }
  s21_vector_free(lineList);
  return uniqueLines;
}

void s21_vector_free(vector_t *vec) {
  if (vec &&
      vec->data) {  // Проверяем, что указатель на вектор и его данные не NULL
    for (size_t i = 0; i < vec->length; i++) {
      free(vec->data[i]);  // Освобождаем каждый элемент массива
    }
    free(vec->data);  // Освобождаем сам массив
  }
}
