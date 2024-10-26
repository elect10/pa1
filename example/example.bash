#!/bin/bash
# The test program for PA1
# Please do not modify this program
# 컴파일
gcc -o pa1 pa1.c
# 테스트 결과를 저장할 파일
result_file="test_results.txt"
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
    if grep -q "$func" pa1.c; then
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
    test_name=$1
    input=$2
   
    echo "Testing $test_name" >> $result_file
    output=$(echo -e "$input" | ./pa1)

    echo "Got: $output"
   
    if [[ "$output" == *"$expected_output"* ]]; then
	echo "PASS" >> $result_file
    else
        echo $expected_output >> 2.txt
        echo $output >> 1.txt
        echo "FAIL" >> $result_file
        echo "Expected: $expected_output" >> $result_file
        echo "Got: $output" >> $result_file
    fi
    echo "" >> $result_file
}
# 기본 테스트 케이스
run_test "ls" "ls .\nquit"
#run_test "ls -al" "ls . -al\nquit" "drwx"
#run_test "head" "head -n 5 pa1.c\nquit" "#include <unistd.h>"
#run_test "tail" "tail -n 5 pa1.c\nquit" "printf(\"%s\", buf);"
#run_test "pwd" "pwd\nquit" "$PWD"
#run_test "invalid command" "invalid_command\nquit" "ERROR: invalid command"
# 복잡한 테스트 케이스
#run_test "Complex Case 1: cp, mv, head" "cp bash.sh test_bash.sh\nmv test_bash.sh renamed_bash.sh\nhead -n 3 renamed_bash.sh\nquit" "#!/bin/bash # The test program for PA1 # Please do not modify this program"
#run_test "Complex Case 2: cp, mv, tail" "cp pa1.c another_copy.c\nmv another_copy.c final_copy.c\ntail -n 3 final_copy.c\nls\nquit" "getcwd(buf, sizeof(buf));\n    printf(\"%s\", buf);\n}\nfinal_copy.c"
#run_test "Complex Case 3: multiple operations" "cp pa1.c test1.c\ncp test1.c test2.c\nmv test1.c test3.c\nls\nhead -n 2 test2.c\ntail -n 2 test3.c\nquit" "test2.c\ntest3.c\n#include <unistd.h>\n#include <fcntl.h>\ngetcwd(buf, sizeof(buf));\n    printf(\"%s\", buf);"
# 결과 출력
cat $result_file
# 점수 계산
total_tests=$(grep -c "Testing" $result_file)
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
rm -f test_copy.c renamed_copy.c another_copy.c final_copy.c test1.c test2.c test3.c 
 
