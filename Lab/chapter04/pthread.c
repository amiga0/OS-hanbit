#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

struct thread_info {
    pthread_t thread_id;       // Thread ID
    int thread_num;            // Thread number
    char *argv_string;         // Argument string
};

static void *thread_fn(void *arg) {
    struct thread_info *tinfo = arg;
    char *uargv, *p;

    printf("Thread %d: top of stack near %p; argv_string=%s\n",
           tinfo->thread_num, &p, tinfo->argv_string);

    uargv = strdup(tinfo->argv_string);
    if (uargv == NULL) {
        perror("strdup");
        pthread_exit(NULL);
    }

    for (p = uargv; *p != '\0'; p++) {
        *p = toupper(*p);
    }

    return uargv; // Return the uppercase string
}

int main(int argc, char *argv[]) {
    int s, tnum, opt, num_threads;
    struct thread_info *tinfo;
    pthread_attr_t attr;
    int stack_size = -1; // Default stack size
    void *res;

    // Parse command-line options
    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
        case 's':
            stack_size = strtoul(optarg, NULL, 0);
            break;
        default:
            fprintf(stderr, "Usage: %s [-s stack-size] arg...\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    num_threads = argc - optind;
    if (num_threads <= 0) {
        fprintf(stderr, "Error: No arguments provided for threads.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize thread attributes
    s = pthread_attr_init(&attr);
    if (s != 0) {
        perror("pthread_attr_init");
        exit(EXIT_FAILURE);
    }

    // Set stack size if specified
    if (stack_size > 0) {
        s = pthread_attr_setstacksize(&attr, stack_size);
        if (s != 0) {
            perror("pthread_attr_setstacksize");
            pthread_attr_destroy(&attr);
            exit(EXIT_FAILURE);
        }
    }

    // Allocate memory for thread_info structures
    tinfo = calloc(num_threads, sizeof(struct thread_info));
    if (tinfo == NULL) {
        perror("calloc");
        pthread_attr_destroy(&attr);
        exit(EXIT_FAILURE);
    }

    // Create threads
    for (tnum = 0; tnum < num_threads; tnum++) {
        tinfo[tnum].thread_num = tnum + 1;
        tinfo[tnum].argv_string = argv[optind + tnum];

        s = pthread_create(&tinfo[tnum].thread_id, &attr, thread_fn, &tinfo[tnum]);
        if (s != 0) {
            perror("pthread_create");
            free(tinfo);
            pthread_attr_destroy(&attr);
            exit(EXIT_FAILURE);
        }
    }

    // Destroy thread attributes object
    s = pthread_attr_destroy(&attr);
    if (s != 0) {
        perror("pthread_attr_destroy");
        free(tinfo);
        exit(EXIT_FAILURE);
    }

    // Join threads and display their return values
    for (tnum = 0; tnum < num_threads; tnum++) {
        s = pthread_join(tinfo[tnum].thread_id, &res);
        if (s != 0) {
            perror("pthread_join");
            free(tinfo);
            exit(EXIT_FAILURE);
        }

        printf("Joined with thread %d; returned value was %s\n",
               tinfo[tnum].thread_num, (char *)res);
        free(res); // Free memory allocated by thread
    }

    // Free allocated memory
    free(tinfo);

    return EXIT_SUCCESS;
}

