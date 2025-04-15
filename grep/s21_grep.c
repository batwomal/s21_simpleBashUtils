#include "s21_grep.h"

int main(int argc, char **argv) {
  grep_settings_t *options = pars_args(argc, argv);

  if (options->error) {
    fprintf(stderr,
            "%s: [-e pattern] [-i] [-v] [-c] [-l] [-n] [-h] [-s] [-f "
            "file] [-o] <file>...",
            NAME);
    return 0;
  }

  for (size_t i = 0; i < options->filesList.length; i++) {
    FILE *file = fopen(options->filesList.data[i], "r");
    options->currentFile = i;
    if (file) {
      grep_file(file, options);
      fclose(file);
    } else if (!options->s_flag)
      s21_error(NAME, "No such file or directory");
  }

  destroy_settings(options);
  return 0;
}

#ifdef __APPLE__
grep_settings_t *pars_args(int argc, char **argv) {
  grep_settings_t *options =
      (grep_settings_t *)s21_calloc(sizeof(grep_settings_t));

  s21_vector_init(&(options->patternList));
  s21_vector_init(&options->filesList);

  vector_t temp;
  s21_vector_init(&temp);

  while (optind < argc) {
    int opt = 0;
    while ((opt = getopt(argc, argv, "e:ivclnshf:o")) != -1) {
      switch (opt) {
        case 'e':
          s21_vector_push_back(&options->patternList, optarg);
          break;
        case 'i':
          options->i_flag = 1;
          break;
        case 'v':
          options->v_flag = 1;
          break;
        case 'c':
          options->c_flag = 1;
          break;
        case 'l':
          options->l_flag = 1;
          break;
        case 'n':
          options->n_flag = 1;
          break;
        case 's':
          options->s_flag = 1;
          break;
        case 'h':
          options->h_flag = 1;
          break;
        case 'f':
          process_f_flag(options, optarg);
          break;
        case 'o':
          options->o_flag = 1;
          break;
        default:
          options->error = 1;
      }
    }

    if (optind < argc) s21_vector_push_back(&temp, argv[optind++]);
  }

  options->patternList = make_unique_lines(&options->patternList);

  if (temp.length) {
    if (options->patternList.length) {
      for (size_t j = 0; j < temp.length; j++)
        s21_vector_push_back(&options->filesList, temp.data[j]);
    } else {
      s21_vector_push_back(&options->patternList, temp.data[0]);
      for (size_t j = 1; j < temp.length; j++)
        s21_vector_push_back(&options->filesList, temp.data[j]);
    }
  }
  s21_vector_free(&temp);
  return options;
}
#endif

#ifdef __linux__
grep_settings_t *pars_args(int argc, char **argv) {
  grep_settings_t *options =
      (grep_settings_t *)s21_calloc(sizeof(grep_settings_t));

  s21_vector_init(&(options->patternList));
  s21_vector_init(&options->filesList);

  int opt = 0;
  while ((opt = getopt(argc, argv, "e:ivclnshf:o")) != -1) {
    switch (opt) {
      case 'e':
        s21_vector_push_back(&options->patternList, optarg);
        break;
      case 'i':
        options->i_flag = 1;
        break;
      case 'v':
        options->v_flag = 1;
        break;
      case 'c':
        options->c_flag = 1;
        break;
      case 'l':
        options->l_flag = 1;
        break;
      case 'n':
        options->n_flag = 1;
        break;
      case 's':
        options->s_flag = 1;
        break;
      case 'h':
        options->h_flag = 1;
        break;
      case 'f':
        process_f_flag(options, optarg);
        break;
      case 'o':
        options->o_flag = 1;
        break;
      default:
        options->error = 1;
    }
  }

  options->patternList = make_unique_lines(&options->patternList);

  if (!options->patternList.length)
    s21_vector_push_back(&options->patternList, argv[optind++]);
  while (optind < argc)
    s21_vector_push_back(&options->filesList, argv[optind++]);
  return options;
}
#endif

void process_f_flag(grep_settings_t *options, const char *path) {
  FILE *file = fopen(path, "r");
  if (file) {
    char line[BUF] = {0};
    while (fgets(line, BUF, file)) {
      size_t len = strlen(line);
      if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';  // Удаляем символ новой строки
      }
      s21_vector_push_back(&options->patternList, line);
    }
    fclose(file);
  } else
    s21_error(NAME, "No such file or directory");
}

void grep_file(FILE *file, grep_settings_t *options) {
  int lineNum = 0;
  int cnt = 0;
  char line[BUF];

  char currentFile[BUF] = {0};
  strcat(currentFile, options->filesList.data[options->currentFile]);
  strcat(currentFile, ":");
  currentFile[0] =
      currentFile[0] * ((options->filesList.length != 1) * !options->h_flag);

  while (fgets(line, BUF, file)) {
    line_data_t *data = grep_line(line, options, &lineNum, &cnt);
    if (data) {
      if (!(options->c_flag || options->l_flag) && data->lineNum != -1)
        print_line(data, options, currentFile);
      destroy_line_data(data);
    }
  }

  if (options->c_flag) {
    printf("%s%d\n", currentFile, cnt);
  } else if (options->l_flag && cnt > 0) {
    printf("%s\n", options->filesList.data[options->currentFile]);
  }
}

line_data_t *grep_line(const char *line, const grep_settings_t *options,
                       int *lineNum, int *cnt) {
  line_data_t *data = (line_data_t *)s21_calloc(sizeof(line_data_t));
  data->line = s21_strdup(line);
  data->matchCntArray =
      (int *)s21_calloc(options->patternList.length * sizeof(int));
  data->lineMatches = NULL;
  (*lineNum)++;
  data->lineNum = -2;
  for (size_t i = 0; i < options->patternList.length; i++) {
    if (strcmp(options->patternList.data[i], "^$") == 0) {
      if ((line[0] == '\n') ^ options->v_flag) {
        data->lineNum = *lineNum;
        (*cnt)++;
      }
      break;
    }

    regex_t *pattern = compile_pattern(options, options->patternList.data[i]);
    if (!pattern) {
      destroy_line_data(data);
      return NULL;
    }
    regmatch_t match = {0};
    const char *cursor = data->line;

    if (options->v_flag ^ regexec(pattern, data->line, 0, &match, 0)) {
      data->lineNum =
          -1 * (data->lineNum == -2) + data->lineNum * (data->lineNum != -2);
    } else {
      while (!(options->v_flag ^ regexec(pattern, cursor, 1, &match, 0)) &&
             *cursor) {
        data->lineMatches = (regmatch_t **)s21_realloc(
            data->lineMatches, ++((data->maxMatchCnt)) * sizeof(regmatch_t *));
        data->lineMatches[data->matchCnt] =
            (regmatch_t *)s21_calloc(sizeof(regmatch_t));
        data->lineMatches[data->matchCnt]->rm_so = match.rm_so;
        data->lineMatches[data->matchCnt]->rm_eo = match.rm_eo;
        cursor += match.rm_eo + 1 * (match.rm_eo == match.rm_so);
        data->matchCnt++;
      }
      data->matchCntArray[i] =
          data->matchCnt - data->matchCntArray[i - 1 * (i != 0)];
      data->lineNum = *lineNum;
      (*cnt)++;
    }
    regfree(pattern);
    free(pattern);
  }
  return data;
}

regex_t *compile_pattern(const grep_settings_t *options, char *pattern) {
  if (pattern == NULL) {
    s21_error(NAME, "No pattern provided");
    return NULL;
  }

  regex_t *regex = (regex_t *)s21_calloc(sizeof(regex_t));

  int reg_flags = (options->i_flag * REG_ICASE) | 0 | REG_NEWLINE;
  if (regcomp(regex, pattern, reg_flags)) {
    regfree(regex);
    free(regex);
    s21_error(NAME, "Compile regular expression error");
    return NULL;
  }

  return regex;
}

void print_line(line_data_t *data, const grep_settings_t *options,
                char *currentFile) {
  if (!options->o_flag) {
    if (options->n_flag) {
      printf("%s%d:%s", currentFile, data->lineNum, data->line);
    } else {
      printf("%s%s", currentFile, data->line);
    }
    if (data->line[strlen(data->line) - 1] != '\n') printf("\n");
  } else
    process_o_flag(data, currentFile, options);
}

void process_o_flag(line_data_t *data, const char *currentFile,
                    const grep_settings_t *options) {
  absolute_index(data, options);
  sort_matches(data->lineMatches, data->matchCnt);
  for (int i = 0; i < data->matchCnt; i++) {
    printf("%s", currentFile);
    for (int j = data->lineMatches[i]->rm_so; j < data->lineMatches[i]->rm_eo;
         j++) {
      putchar(data->line[j]);
    }
    putchar('\n');
  }
}

void absolute_index(line_data_t *data, const grep_settings_t *options) {
  // int cnt = 0;
  // for (size_t j = 0; j < options->patternList.length; j++) {
  //   for (int i = 1; i < data->matchCntArray[j] && cnt < data->matchCnt; i++)
  //   {
  //     int len = data->lineMatches[i + cnt * j]->rm_eo -
  //               data->lineMatches[i + cnt * j]->rm_so;
  //     data->lineMatches[i + cnt * j]->rm_so +=
  //         data->lineMatches[(i + cnt * j) - 1]->rm_eo;
  //     data->lineMatches[i + cnt * j]->rm_eo =
  //         data->lineMatches[i + cnt * j]->rm_so + len;
  //   }
  //   cnt += data->matchCntArray[j];
  // }
  int cnt = 0;
  for (size_t i = 0; i < options->patternList.length; i++) {
    int arrLengh = data->matchCntArray[i];
    for (int j = 1; j < arrLengh && j + cnt < data->matchCnt; j++) {
      int len =
          data->lineMatches[j + cnt]->rm_eo - data->lineMatches[j + cnt]->rm_so;
      data->lineMatches[j + cnt]->rm_so +=
          data->lineMatches[j + cnt - 1]->rm_eo;
      data->lineMatches[j + cnt]->rm_eo =
          data->lineMatches[j + cnt]->rm_so + len;
    }
    cnt += arrLengh;
  }
}

void sort_matches(regmatch_t **matches, int matchCnt) {
  qsort(matches, matchCnt, sizeof(regmatch_t *), compare_matches);
}

int compare_matches(const void *a, const void *b) {
  const regmatch_t *match_a = *(const regmatch_t **)a;
  const regmatch_t *match_b = *(const regmatch_t **)b;
  return 1 * (match_a->rm_so > match_b->rm_so) -
         (match_a->rm_so < match_b->rm_so);
}

void destroy_settings(grep_settings_t *options) {
  if (options) {
    if (options->patternList.data) {
      s21_vector_free(&options->patternList);
    }

    if (options->filesList.data) {
      s21_vector_free(&options->filesList);
    }

    if (options->f_value) {
      free(options->f_value);
    }

    free(options);
  }
}

void destroy_line_data(line_data_t *lineData) {
  if (lineData->line) {
    free(lineData->line);
    if (lineData->matchCntArray) free(lineData->matchCntArray);
    if (lineData->lineMatches) {
      for (int i = 0; i < lineData->maxMatchCnt; i++) {
        free(lineData->lineMatches[i]);
      }
      free(lineData->lineMatches);
    }
    free(lineData);
  }
}
