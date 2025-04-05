#include <stdio.h>

// 재귀적으로 팩토리얼 계산
int foo(int val) {
    if (val <= 1) return 1; // 기본 조건
    return (foo(val - 1) * val); // 재귀 호출
}
