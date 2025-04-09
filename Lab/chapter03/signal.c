/* signal test */
/* sigaction */
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h> // For pause()

void signal_handler(int signo);
int count = 0;

int main()
{
    struct sigaction old_sa;
    struct sigaction new_sa;
    memset(&new_sa, 0, sizeof(new_sa));
    new_sa.sa_handler = &signal_handler;
    sigaction(SIGALRM, &new_sa, &old_sa);

    struct itimerval new_itimer, old_itimer;
    new_itimer.it_interval.tv_sec = 1;
    new_itimer.it_interval.tv_usec = 0;
    new_itimer.it_value.tv_sec = 1;
    new_itimer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &new_itimer, &old_itimer);

    while (1) {
        pause(); // Wait for signals
    }
    return 0;
}

void signal_handler(int signo)
{
    printf("Signal received! signo: %d\n", signo);
    count++;

    if (count == 3) {
        printf("Exiting after receiving 3 signals.\n");
        exit(0);
    }
}












