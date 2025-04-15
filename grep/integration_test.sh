#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'
BLUE='\033[0;34m'

RESULTS_DIR="test_results"
TOTAL_TESTS=0
PASSED_TESTS=0

# Убедимся, что директория для результатов существует
mkdir -p "$RESULTS_DIR"

run_test() {
    local test_name="$1"
    local pattern="$2"
    local flags="$3"
    local files="$4"
    ((TOTAL_TESTS++))

    # Подготовка директории для текущего теста
    local sanitized_name=$(echo "$test_name")
    local test_dir="$RESULTS_DIR/$sanitized_name"
    mkdir -p "$test_dir"
    
    echo -e "${BLUE}Running test: ${test_name}${NC}"

    # Выполнение команды
    if [ -z "$flags" ]; then
        ./s21_grep "$pattern" $files > "$test_dir/s21_output.txt"
        grep "$pattern" $files > "$test_dir/original_output.txt"
    else
        ./s21_grep $flags "$pattern" $files > "$test_dir/s21_output.txt"
        grep $flags "$pattern" $files > "$test_dir/original_output.txt"
    fi

    # Сравнение результатов
    if diff "$test_dir/s21_output.txt" "$test_dir/original_output.txt" > "$test_dir/diff.txt" 2>&1; then
        echo -e "${GREEN}Test passed: ${test_name}${NC}"
        ((PASSED_TESTS++))
    else
        echo -e "${RED}Test failed: ${test_name}${NC}"
        echo "Differences found. See $test_dir/diff.txt for details."
    fi
    echo "----------------------------------------"
}

# Запуск тестов
run_test "Basic pattern match" "test" "" "test.txt"
run_test "Case sensitive" "TEST" "" "test.txt"
run_test "Multiple files" "test" "" "test.txt test2.txt"

# Тесты с флагами
run_test "Flag -e (template)" "test" "-e [^hello$]" "test.txt"
run_test "Flag -i (ignore case)" "test" "-i" "test.txt"
run_test "Flag -v (invert match)" "test" "-v" "test.txt"
run_test "Flag -c (count)" "test" "-c" "test.txt"
run_test "Flag -l (files with matches)" "test" "-l" "test.txt test2.txt"
run_test "Flag -n (line number)" "test" "-n" "test.txt"
run_test "Flag -h (no filename)" "test" "-h" "test.txt test2.txt"
run_test "Flag -s (suppress errors)" "test" "-s" "test.txt nonexistent.txt"
run_test "Flag -f (patterns from file)" "" "-f patterns.txt" "test.txt"
run_test "Flag -o (only matching)" "test" "-o" "test.txt"

# Скомбинированные флаги
run_test "Flags -iv" "test" "-iv" "test.txt"
run_test "Flags -in" "test" "-in" "test.txt"
run_test "Flags -ic" "test" "-ic" "test.txt"
run_test "Flags -il" "test" "-il" "test.txt test2.txt"
run_test "Flags -nh" "test" "-nh" "test.txt test2.txt"
run_test "Flags -cl" "test" "-cl" "test.txt test2.txt"

# Разные паттерны
run_test "Special chars pattern" "\\." "" "test.txt"
run_test "Number pattern" "123" "" "test.txt"
run_test "Multiple word pattern" "test patterns" "" "test.txt"

# Вывод итогов
echo -e "\n${BLUE}Test Summary:${NC}"
echo -e "Total tests: ${TOTAL_TESTS}"
echo -e "${GREEN}Passed tests: ${PASSED_TESTS}${NC}"
if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo -e "${GREEN}All tests passed successfully!${NC}"
else
    echo -e "${RED}Failed tests: $(($TOTAL_TESTS - $PASSED_TESTS))${NC}"
    exit 1
fi
