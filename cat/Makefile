.SILENT:
CC = gcc

CFLAGS = -Wall -Werror -Wextra -std=c11 -pedantic -g

MAIN_C = s21_cat.c

OUTPATH = s21_cat

all: s21_cat

clang:
	cp ../../materials/linters/.clang-format ./
	clang-format -i *.c *.h

check:
	cp ../../materials/linters/.clang-format ./
	clang-format -n *.c *.h
	cppcheck --enable=all --suppress=missingIncludeSystem *.c


rebuild: clean all

tests: rebuild
	bash integration_test.sh

s21_cat: $(MAIN_C)
	$(CC) $(CFLAGS) $(MAIN_C) -o $(OUTPATH)

clean:
	rm -rf s21_cat
	rm -rf test_results