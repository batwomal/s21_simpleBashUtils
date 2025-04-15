.SILENT:
CC = gcc

CFLAGS = -Wall -Werror -Wextra -std=c11 -pedantic -g

MAIN_C = s21_grep.c

COMMON = ../common/*.c

OUTPATH = s21_grep

all: s21_grep

clang:
	cp ../../materials/linters/.clang-format ./
	clang-format -i ../*/*.c ../*/*.h

check:
	cp ../../materials/linters/.clang-format ./
	clang-format -n ../*/*.c ../*/*.h
	cppcheck --enable=all --suppress=missingIncludeSystem ../*/*.c
	

rebuild: clean all

tests: rebuild
	bash integration_test.sh

s21_grep: $(MAIN_C)
	$(CC) $(CFLAGS) $(COMMON) $(MAIN_C) -o $(OUTPATH) 

clean:
	rm -rf s21_grep
	rm -rf test_results