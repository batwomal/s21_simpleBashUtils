#!/bin/bash

# Функция для выполнения тестов и сравнения выводов
run_test() {
    local pattern="$1"
    local file="$2"
    local flags="$3"
    
    # Выполнение команды grep
    grep_output=$(grep $flags "$pattern" "$file")
    
    # Выполнение команды s21_grep
    s21_grep_output=$(./s21_grep $flags "$pattern" "$file")
    
    # Сравнение выводов
    if [ "$grep_output" == "$s21_grep_output" ]; then
        return 0  # Успешно
    else
        return 1  # Неудачно
    fi
}

# Массив тестовых данных
test_files=("test_file.txt" "test_file_2.txt" "test_file_3.txt")
patterns=("hello" "WORLD" "apple" "quick" "banana")
flags=("-e" "-i" "-v" "-c" "-l" "-n" "-e -i" "-e -v" "-e -c" "-e -l" "-e -n" "-i -v" "-i -c" "-i -l" "-i -n" "-v -c" "-v -l" "-v -n" "-c -l" "-c -n" "-l -n" "-e -i -v" "-e -i -c" "-e -i -l" "-e -i -n" "-e -v -c" "-e -v -l" "-e -v -n" "-e -c -l" "-e -c -n" "-e -l -n" "-i -v -c" "-i -v -l" "-i -v -n" "-i -c -l" "-i -c -n" "-i -l -n" "-v -c -l" "-v -c -n" "-v -l -n" "-c -l -n" "-e -i -v -c" "-e -i -v -l" "-e -i -v -n" "-e -i -c -l" "-e -i -c -n" "-e -i -l -n" "-e -v -c -l" "-e -v -c -n" "-e -v -l -n" "-e -c -l -n" "-i -v -c -l" "-i -v -c -n" "-i -v -l -n" "-i -c -l -n" "-v -c -l -n" "-e -i -v -c -l" "-e -i -v -c -n" "-e -i -v -l -n" "-e -i -c -l -n" "-e -v -c -l -n" "-i -v -c -l -n")

# Подготовка тестовых данных
echo -e "hello world\nHello World\nHELLO WORLD\nhello\nWORLD\n" > test_file.txt
echo -e "apple\nbanana\ncherry\n" > test_file_2.txt
echo -e "The quick brown fox jumps over the lazy dog\n" > test_file_3.txt

# Счётчики
total_tests=0
passed_tests=0
failed_tests=0
failed_test_cases=()

# Основной цикл тестирования
for file in "${test_files[@]}"; do
    for pattern in "${patterns[@]}"; do
        for flag in "${flags[@]}"; do
            ((total_tests++))
            if run_test "$pattern" "$file" "$flag"; then
                ((passed_tests++))
            else
                ((failed_tests++))
                failed_test_cases+=("Pattern: $pattern, File: $file, Flags: $flag")
            fi
        done
    done
done

# Вывод результатов
echo "Total tests: $total_tests"
echo "Passed tests: $passed_tests"
echo "Failed tests: $failed_tests"
echo "Failed test cases:"
for failed_case in "${failed_test_cases[@]}"; do
    echo "$failed_case"
done
