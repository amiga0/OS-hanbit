#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct sharedobject {
    FILE *rfile;
    int linenum;
    char *line;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int full;
    int done;
} so_t;

void *producer(void *arg) {
    so_t *so = arg;
    int *ret = malloc(sizeof(int));
    if (!ret) {
        perror("malloc");
        pthread_exit(NULL);
    }

    FILE *rfile = so->rfile;
    int line_count = 0;
    char *line = NULL;
    size_t len = 0;

    while (1) {
        pthread_mutex_lock(&so->lock);

        // 생산자가 데이터를 소비할 때까지 대기
        while (so->full) {
            pthread_cond_wait(&so->cond, &so->lock);
        }

        ssize_t read = getdelim(&line, &len, '\n', rfile);
        if (read == -1) {
            so->done = 1; // 파일 끝
            pthread_cond_broadcast(&so->cond);
            pthread_mutex_unlock(&so->lock);
            break;
        }

        so->linenum = line_count++;
        so->line = strdup(line); // 공유 데이터 복사
        so->full = 1;

        pthread_cond_signal(&so->cond);
        pthread_mutex_unlock(&so->lock);
    }

    free(line);
    printf("Producer_%x: %d lines\n", (unsigned int)pthread_self(), line_count);
    *ret = line_count;
    pthread_exit(ret);
}

void *consumer(void *arg) {
    so_t *so = arg;
    int *ret = malloc(sizeof(int));
    if (!ret) {
        perror("malloc");
        pthread_exit(NULL);
    }

    int line_count = 0;

    while (1) {
        pthread_mutex_lock(&so->lock);

        // 소비자가 데이터를 생산할 때까지 대기
        while (!so->full && !so->done) {
            pthread_cond_wait(&so->cond, &so->lock);
        }

        if (so->done && !so->full) {
            pthread_mutex_unlock(&so->lock);
            break;
        }

        char *line = so->line;
        int linenum = so->linenum;

        printf("Consumer_%x: [%02d] %s", (unsigned int)pthread_self(), linenum, line);
        free(so->line);
        so->line = NULL;
        so->full = 0;

        pthread_cond_signal(&so->cond);
        pthread_mutex_unlock(&so->lock);

        line_count++;
    }

    printf("Consumer_%x: %d lines\n", (unsigned int)pthread_self(), line_count);
    *ret = line_count;
    pthread_exit(ret);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./prod_cons <readfile> [#Producer] [#Consumer]\n");
        exit(EXIT_FAILURE);
    }

    int Nprod = (argc > 2) ? atoi(argv[2]) : 1;
    int Ncons = (argc > 3) ? atoi(argv[3]) : 1;
    if (Nprod > 100) Nprod = 100;
    if (Ncons > 100) Ncons = 100;

    FILE *rfile = fopen(argv[1], "r");
    if (!rfile) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    so_t *share = malloc(sizeof(so_t));
    if (!share) {
        perror("malloc");
        fclose(rfile);
        exit(EXIT_FAILURE);
    }

    memset(share, 0, sizeof(so_t));
    share->rfile = rfile;
    pthread_mutex_init(&share->lock, NULL);
    pthread_cond_init(&share->cond, NULL);

    pthread_t prod[Nprod], cons[Ncons];
    for (int i = 0; i < Nprod; i++) {
        if (pthread_create(&prod[i], NULL, producer, share) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < Ncons; i++) {
        if (pthread_create(&cons[i], NULL, consumer, share) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < Nprod; i++) {
        int *ret;
        if (pthread_join(prod[i], (void **)&ret) == 0) {
            printf("Main: Producer_%d joined with %d\n", i, *ret);
            free(ret);
        }
    }
    for (int i = 0; i < Ncons; i++) {
        int *ret;
        if (pthread_join(cons[i], (void **)&ret) == 0) {
            printf("Main: Consumer_%d joined with %d\n", i, *ret);
            free(ret);
        }
    }

    pthread_mutex_destroy(&share->lock);
    pthread_cond_destroy(&share->cond);
    fclose(rfile);
    free(share);

    return 0;
}

