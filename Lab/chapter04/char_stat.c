#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_STRING_LENGTH 30
#define ASCII_SIZE 256
#define SEPARATORS "{}()[],;\" \n\t^"

int stat[MAX_STRING_LENGTH];
int stat2[ASCII_SIZE];

int main(int argc, char *argv[]) {
    int i = 0, rc = 0, sum = 0, line_num = 1;
    char *line = NULL;
    size_t length = 0;
    FILE *rfile = NULL;

    if (argc != 2) {
        fprintf(stderr, "Usage: ./stat <filename>\n");
        return 1;
    }

    // 파일 열기
    rfile = fopen(argv[1], "rb");
    if (rfile == NULL) {
        perror(argv[1]);
        return 1;
    }

    // 배열 초기화
    memset(stat, 0, sizeof(stat));
    memset(stat2, 0, sizeof(stat2));

    while (1) {
        char *cptr = NULL, *substr = NULL, *brka = NULL;

        // 한 줄 읽기
        rc = getdelim(&line, &length, '\n', rfile);
        if (rc == -1) break;

        cptr = line;
#ifdef _IO_
        printf("[%3d] %s\n", line_num++, line);
#endif
        for (substr = strtok_r(cptr, SEPARATORS, &brka); substr; substr = strtok_r(NULL, SEPARATORS, &brka)) {
            size_t substr_length = strlen(substr);

            // 문자열 길이 통계 업데이트
            if (substr_length >= MAX_STRING_LENGTH) substr_length = MAX_STRING_LENGTH;
#ifdef _IO_
            printf("length: %zu\n", substr_length);
#endif
            stat[substr_length - 1]++;

            // 각 문자 빈도 통계 업데이트
            for (size_t j = 0; j < substr_length; j++) {
                unsigned char ch = (unsigned char)substr[j];
                if (ch < ASCII_SIZE) {
                    stat2[ch]++;
#ifdef _IO_
                    printf("# of %c(%d): %d\n", ch, ch, stat2[ch]);
#endif
                }
            }
        }
    }

    // 총합 계산
    sum = 0;
    for (i = 0; i < MAX_STRING_LENGTH; i++) {
        sum += stat[i];
    }

    // 분포 출력
    printf("*** Print out distributions ***\n");
    printf("  #ch  freq\n");
    for (i = 0; i < MAX_STRING_LENGTH; i++) {
        int num_star = (sum > 0) ? stat[i] * 80 / sum : 0;
        printf("[%3d]: %4d \t", i + 1, stat[i]);
        for (int j = 0; j < num_star; j++) printf("*");
        printf("\n");
    }

    // 알파벳 빈도 출력
    printf("       A        B        C        D        E        F        G        H        I        J        K        L        M        N        O        P        Q        R        S        T        U        V        W        X        Y        Z\n");
    printf("%8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d %8d\n",
           stat2['A'] + stat2['a'], stat2['B'] + stat2['b'], stat2['C'] + stat2['c'], stat2['D'] + stat2['d'], stat2['E'] + stat2['e'],
           stat2['F'] + stat2['f'], stat2['G'] + stat2['g'], stat2['H'] + stat2['h'], stat2['I'] + stat2['i'], stat2['J'] + stat2['j'],
           stat2['K'] + stat2['k'], stat2['L'] + stat2['l'], stat2['M'] + stat2['m'], stat2['N'] + stat2['n'], stat2['O'] + stat2['o'],
           stat2['P'] + stat2['p'], stat2['Q'] + stat2['q'], stat2['R'] + stat2['r'], stat2['S'] + stat2['s'], stat2['T'] + stat2['t'],
           stat2['U'] + stat2['u'], stat2['V'] + stat2['v'], stat2['W'] + stat2['w'], stat2['X'] + stat2['x'], stat2['Y'] + stat2['y'],
           stat2['Z'] + stat2['z']);

    // 메모리 해제 및 파일 닫기
    free(line);
    fclose(rfile);

    return 0;
}
