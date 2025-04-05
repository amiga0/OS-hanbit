#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int foo(int val);

int main(int argc, char *argv[]) {
    int res;
    int val = 3; // 기본값

    // 입력값 처리
    if (argc == 2) {
        val = atoi(argv[1]);
        if (val < 0) {
            fprintf(stderr, "Error: 입력값은 음수가 될 수 없습니다.\n");
            return 1;
        }
        if (val > 12) { // 12! 이상은 int 범위를 초과할 수 있음
            fprintf(stderr, "Error: 입력값이 너무 큽니다. 12 이하의 값을 입력하세요.\n");
            return 1;
        }
    }

    // 팩토리얼 계산
    res = foo(val);
    printf("%d-factorial: %d\n", val, res);
    return 0;
}
