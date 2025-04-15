#include "s21_cat.h"

int main(int argc, char **argv) {
  options settings = process_flags(argc, argv);
  if (settings.error) {
    return 0;
  }

  FILE *file = NULL;
  char block[BUF] = {0};
  int lineNum = 1;
  int lastBlankLine = 0;
  int firstBlock = 1;
  int isEOF = 0;
  if (argc == 1) {
    while (fgets(block, BUF, stdin)) {
      isEOF = feof(stdin);
      print_line(block, firstBlock, isEOF, &lineNum, &lastBlankLine, &settings);
    }
  } else {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] != '-') {
        file = fopen(argv[i], "r");
        if (file == NULL) {
          fprintf(stderr, "%s: %s: No such file or directory\n", NAME, argv[i]);
          continue;
        }

        while (fgets(block, BUF, file)) {
          int lastBlock = !!strchr(block, '\n');
          isEOF = feof(file);
          print_line(block, firstBlock, isEOF, &lineNum, &lastBlankLine,
                     &settings);
          firstBlock = lastBlock ? 1 : 0;
        }
        fclose(file);
      }
    }
  }
  return 0;
}

options process_flags(int argc, char **argv) {
  options settings = {0, 0, 0, 0, 0, 0, 0};

  static struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {"show-nonprinting", no_argument, NULL, 'v'},
      {"show-ends", no_argument, NULL, 'E'},
      {"show-tabs", no_argument, NULL, 'T'},
      {0, 0, 0, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "bneTtsvE", long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'b':
        settings.number_nonblank = 1;
        break;
      case 'n':
        settings.number = 1;
        break;
      case 'e':
        settings.show_ends = 1;
        settings.show_visible = 1;
        break;
      case 't':
        settings.show_tabs = 1;
        settings.show_visible = 1;
        break;
      case 's':
        settings.squeeze_blank = 1;
        break;
      case 'v':
        settings.show_visible = 1;
        break;
      case 'E':
        settings.show_ends = 1;
        break;
      case 'T':
        settings.show_tabs = 1;
        break;
      case '?':
        settings.error = 1;
        break;
      default:
        settings.error = 1;
        break;
    }
  }
  return settings;
}

void print_line(char *block, int firstBlock, int isEOF, int *lineNum,
                int *lastBlankLine, const options *settings) {
  int isBlank = (block[0] == '\n' && firstBlock);

  if (!(settings->squeeze_blank && isBlank && *lastBlankLine)) {
    *lastBlankLine = isBlank;

    if (settings->number_nonblank && !isBlank && firstBlock) {
      printf("%6d\t", *lineNum);
      (*lineNum)++;
    } else if (settings->number && !settings->number_nonblank && firstBlock) {
      printf("%6d\t", *lineNum);
      (*lineNum)++;
    }
    print_block(block, isEOF, isBlank, settings);
  }
}

void print_block(char *block, int isEOF, int isBlank, const options *settings) {
  int isEOL = 0;
  for (int i = 0; i < BUF - 1 && !isEOL && !(isEOF && block[i] == '\0'); i++) {
    unsigned char ch = (unsigned char)block[i];
    if (settings->show_tabs && ch == '\t')
      printf("^I");
    else if (!settings->number_nonblank && settings->show_ends && ch == '\n')
      printf("$\n");
    else if (settings->number_nonblank && settings->show_ends && isBlank &&
             ch == '\n') {
      ;
#ifdef __APPLE__
      printf("      \t$\n");
#endif
#ifdef __linux__
      printf("$\n");
#endif

    } else if (settings->number_nonblank && settings->show_ends && !isBlank &&
               ch == '\n')
      printf("$\n");
    else if (settings->show_visible && (ch < 32 || ch > 126) && ch != '\t' &&
             ch != '\n')
      print_extended(ch);
    else
      printf("%c", ch);
    isEOL = ch == '\n';
    block[i] = 0;
  }
}

void print_extended(unsigned char ch) {
  if (ch < 32)
    printf("^%c", ch + 64);
  else if (ch == 127)
    printf("^?");
  else if (ch < 160)
    printf("M-^%c", ch - 64);
#ifdef __linux__
  else if (ch == 255)
    printf("M-^?");
  else
    printf("M-%c", ch - 128);
#endif
#ifdef __APPLE__
  else printf("%c", ch);
#endif
}
