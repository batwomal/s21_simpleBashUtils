#ifndef S21_CAT_H
#define S21_CAT_H

#include <getopt.h>
#include <stdio.h>
#include <string.h>

#define NAME "s21_cat"
#define BUF 2

typedef struct options {
  int number;
  int number_nonblank;
  int squeeze_blank;
  int show_ends;
  int show_tabs;
  int show_visible;
  int error;
} options;

options process_flags(int argc, char **argv);
void print_line(char *block, int firstBlock, int isEOF, int *lineNum,
                int *lastBlankLine, const options *settings);
void print_block(char *block, int isEOF, int isBlank, const options *settings);
void print_extended(unsigned char ch);

#endif
