#!/bin/bash
# 컴파일
test_file="./pa1_skeleton.c"
gcc -o pa1 $test_file
# 테스트 결과를 저장할 파일
result_file="./test_results.txt"
> $result_file
# string.h 함수 사용 체크
echo "Checking for string.h functions usage:" >> $result_file
string_functions=(
    "strcat" "strncat" "strchr" "strrchr" "strcmp" "strncmp" "strcoll" "strcpy" "strncpy"
    "strerror" "strlen" "strpbrk" "strspn" "strstr" "strtok" "strxfrm" "memchr" "memcmp"
    "memcpy" "memmove" "memset"
)
used_functions=()
for func in "${string_functions[@]}"; do
    if grep -q "$func" $test_file; then
        used_functions+=("$func")
    fi
done
if [ ${#used_functions[@]} -eq 0 ]; then
    echo "No string.h functions found. PASS" >> $result_file
else
    echo "The following string.h functions were found: ${used_functions[*]}" >> $result_file
    echo "FAIL" >> $result_file
fi
echo "" >> $result_file
# 테스트 함수
run_test() {
    test_number=$1
    test_name=$2
    input=$3
    
    echo "Test Case $test_number: $test_name" >> $result_file
    output=$(echo -e "$input" | ./pa1)
    
    echo "$output" > "output/answer$test_number.txt"
    
    if diff --ignore-all-space "output/answer$test_number.txt" "testcase/testcase$test_number.txt" >/dev/null; then
        echo "PASS" >> $result_file
    else
        echo "FAIL" >> $result_file
        echo "Expected:" >> $result_file
        cat "testcase/testcase$test_number.txt" >> $result_file
        echo "Got:" >> $result_file
        cat "output/answer$test_number.txt" >> $result_file
    fi
    echo "" >> $result_file
}
# 테스트 케이스
ls "./example" | tr '\n' ' '  > "testcase/testcase1.txt"
run_test 1 "ls" "ls ./example\nquit"
ls -al "./example" > "testcase/testcase2.txt"
run_test 2 "ls -al" "ls ./example -al\nquit"
head -n 5 "./example/e.sh" > "testcase/testcase3.txt"
run_test 3 "head" "head -n 5 ./example/e.sh\nquit"
tail -n 5 "./example/e.sh" > "testcase/testcase4.txt"
run_test 4 "tail" "tail -n 5 ./example/e.sh\nquit"
pwd  > "testcase/testcase5.txt"
run_test 5 "pwd" "pwd\nquit"
echo "ERROR: invalid command" > "testcase/testcase6.txt"
run_test 6 "invalid command" "invalid_command\nquit"
echo "ERROR: invalid path" > "testcase/testcase7.txt"
run_test 7 "invalid path" "mv ex/example.bash example/example.bash\nquit"
echo "ERROR: The command is executed abnormally" > "testcase/testcase8.txt"
run_test 8 "abnormally executed command" "ls -al\nquit"
# 복잡한 테스트 케이스
run_test 9 "Complex Case 1: cp, mv, head" "cp example/e.sh output/copied_e.sh\nmv output/copied_e.sh output/renamed_e.sh\nhead -n 3 output/renamed_e.sh\nquit"
run_test 10 "Complex Case 2: cp, mv, tail, ls" "cp example/e.sh output/another_e.sh\nmv output/another_e.sh output/final_e.sh\ntail -n 3 output/final_e.sh\nls ./output\nquit"
run_test 11 "Complex Case 2: cp, mv, head, tail, ls" "cp example/e.sh output/another_e.sh\nmv output/another_e.sh output/final_e.sh\nhead -n 7 output/final_e.sh\ntail -n 2 output/final_e.sh\nls ./output\nquit"
# 결과 출력
cat $result_file
# 점수 계산
total_tests=$(grep -c "Test Case" $result_file)
passed_tests=$(grep -c "PASS" $result_file)
score=$((passed_tests * 100 / (total_tests + 1)))  # +1 for string.h check
echo "Total tests: $((total_tests + 1))"  # +1 for string.h check
echo "Passed tests: $passed_tests"
echo "Score: $score%"
# string.h 함수 사용에 대한 감점
if [ ${#used_functions[@]} -gt 0 ]; then
    penalty=$((${#used_functions[@]} * 5))
    echo "Penalty for using string.h functions: -$penalty%"
    score=$((score - penalty))
    echo "Final score after penalty: $score%"
fi
# 테스트 파일 정리
rm -f output/renamed_e.sh output/final_e.sh output/another_e.sh output/answer*
