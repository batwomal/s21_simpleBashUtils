#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/common.h"
#include "../common/s21_vector.h"

#define NAME "grep"
#define BUF 6144

typedef struct {
  vector_t filesList;
  int currentFile;
  vector_t patternList;
  int i_flag;
  int v_flag;
  int c_flag;
  int l_flag;
  int n_flag;
  int h_flag;
  int s_flag;
  int f_flag;
  char *f_value;
  int o_flag;
  int error;
} grep_settings_t;

typedef struct line_data_t {
  char *line;
  int lineNum;
  int lineCnt;
  int matchCnt;
  int *matchCntArray;
  int maxMatchCnt;
  regmatch_t **lineMatches;
} line_data_t;

void destroy_settings(grep_settings_t *options);
void destroy_line_data(line_data_t *lineData);

grep_settings_t *pars_args(int argc, char **argv);
void process_f_flag(grep_settings_t *options, const char *path);
void grep_file(FILE *file, grep_settings_t *options);
line_data_t *grep_line(const char *line, const grep_settings_t *options,
                       int *lineNum, int *cnt);
regex_t *compile_pattern(const grep_settings_t *options, char *pattern);
void print_line(line_data_t *data, const grep_settings_t *options,
                char *currentFile);
void process_o_flag(line_data_t *data, const char *currentFile,
                    const grep_settings_t *options);
void absolute_index(line_data_t *data, const grep_settings_t *options);

void sort_matches(regmatch_t **matches, int matchCnt);
int compare_matches(const void *a, const void *b);

#endif
