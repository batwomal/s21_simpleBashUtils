#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'
BLUE='\033[0;34m'

RESULTS_DIR="test_results"
PASSED_TESTS_DIR="$RESULTS_DIR/passed"
FAILED_TESTS_DIR="$RESULTS_DIR/failed"
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

PASSED_TESTS_LIST=()
FAILED_TESTS_LIST=()

PROGRAM="./s21_cat"
if [ "$(uname)" == "Darwin" ]; then
    TEST_FILES="test_cases_cat_macos"  # Основной файл тестов без 138 символа asci, для которого есть баг у оригинального cat для macos
else 
    TEST_FILES="test_cases_cat" # Основной файл тестов 
fi
EMPTY_TEST_FILE="test_cases_cat_empty"  # Пустой файл для тестов
NONEXISTENT_TEST_FILE="test_cases_cat_NAN"  # Некорректный файл для тестов
SECOND_TEST_FILE="test_cases_cat2"  # Дополнительный тестовый файл

rm -rf "$RESULTS_DIR"
mkdir -p "$PASSED_TESTS_DIR" "$FAILED_TESTS_DIR"

run_test() {
    local test_name="$1"
    local flags="$2"
    shift 2
    local input_files=("$@")
    local test_dir="$RESULTS_DIR/$test_name"

    mkdir -p "$test_dir"
    ((TOTAL_TESTS++))

    echo -e "${BLUE}Running test: ${test_name}${NC}"

    if [ -z "$flags" ]; then
        $PROGRAM "${input_files[@]}" > "$test_dir/s21_output.txt"
        cat "${input_files[@]}" > "$test_dir/original_output.txt"
    else
        $PROGRAM $flags "${input_files[@]}" > "$test_dir/s21_output.txt"
        cat $flags "${input_files[@]}" > "$test_dir/original_output.txt"
    fi

    if diff "$test_dir/s21_output.txt" "$test_dir/original_output.txt" >/dev/null 2>&1; then
        echo -e "${GREEN}Test passed: ${test_name}${NC}"
        ((PASSED_TESTS++))
        PASSED_TESTS_LIST+=("${test_name}")
        
        mv "$test_dir" "$PASSED_TESTS_DIR/"

    else
        echo -e "${RED}Test failed: ${test_name}${NC}"
        echo "Differences found:"
        diff "$test_dir/s21_output.txt" "$test_dir/original_output.txt"
        ((FAILED_TESTS++))
        FAILED_TESTS_LIST+=("${test_name}")

        mv "$test_dir" "$FAILED_TESTS_DIR/"
    fi
    echo "----------------------------------------"
}

run_test "No flags" "" "$TEST_FILES"
run_test "Flag -b" "-b" "$TEST_FILES"
run_test "Flag -e" "-e" "$TEST_FILES"
run_test "Flag -n" "-n" "$TEST_FILES"
run_test "Flag -s" "-s" "$TEST_FILES"
run_test "Flag -t" "-t" "$TEST_FILES"
run_test "Flag -v" "-v" "$TEST_FILES"
run_test "Flags -b -e" "-b -e" "$TEST_FILES"
run_test "Flags -b -n" "-b -n" "$TEST_FILES"
run_test "Flags -b -s" "-b -s" "$TEST_FILES"
run_test "Flags -b -t" "-b -t" "$TEST_FILES"
run_test "Flags -e -n" "-e -n" "$TEST_FILES"
run_test "Flags -e -s" "-e -s" "$TEST_FILES"
run_test "Flags -e -t" "-e -t" "$TEST_FILES"
run_test "Flags -n -s" "-n -s" "$TEST_FILES"
run_test "Flags -n -t" "-n -t" "$TEST_FILES"
run_test "Flags -s -t" "-s -t" "$TEST_FILES"
run_test "Flags -b -e -n" "-b -e -n" "$TEST_FILES"
run_test "Flags -b -e -s" "-b -e -s" "$TEST_FILES"
run_test "Flags -b -e -t" "-b -e -t" "$TEST_FILES"
run_test "Flags -b -n -s" "-b -n -s" "$TEST_FILES"
run_test "Flags -e -n -s" "-e -n -s" "$TEST_FILES"
run_test "Flags -n -s -t" "-n -s -t" "$TEST_FILES"
run_test "All flags" "-b -e -n -s -t" "$TEST_FILES"
run_test "All flags connected" "-benst" "$TEST_FILES"

if [ "$(uname)" != "Darwin" ]; then
run_test "Flag -E" "-E" "$TEST_FILES"
run_test "Flag -T" "-T" "$TEST_FILES"
run_test "Flag --number" "--number" "$TEST_FILES"
run_test "Flag --squeeze-blank" "--squeeze-blank" "$TEST_FILES"
run_test "Flag --number-nonblank" "--number-nonblank" "$TEST_FILES"
fi

run_test "Empty file" "" "$EMPTY_TEST_FILE"
run_test "Nonexistent file" "" "$NONEXISTENT_TEST_FILE"
run_test "Multiple files" "" "$TEST_FILES" "$SECOND_TEST_FILE"
run_test "Invalid flag" "-z" "$TEST_FILES"

echo -e "\n${BLUE}Test Summary:${NC}"
echo -e "Total tests: ${TOTAL_TESTS}"

echo -e "\n${BLUE}Test Summary:${NC}"
echo -e "Total tests: ${TOTAL_TESTS}"
echo -e "${GREEN}Passed tests: ${PASSED_TESTS}${NC}"

if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo -e "${GREEN}All tests passed successfully!${NC}"
else
    echo -e "${RED}Failed tests: ${FAILED_TESTS}${NC}"
    echo -e "Failed tests details: ${RED}"
    for test in "${FAILED_TESTS_LIST[@]}"; do
        echo -e "- $test"
    done
    exit 1
fi
