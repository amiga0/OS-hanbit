#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg.h"
#include <string.h>
#include <unistd.h>
#include <errno.h> // For error handling

#define MSGQ_KEY 0x12345 // Define key as a constant

int main()
{
    int msgq;
    int ret;

    // Create or get the message queue
    msgq = msgget(MSGQ_KEY, IPC_CREAT | 0666);
    if (msgq == -1) {
        perror("msgget failed");
        return 1;
    }
    printf("msgq id: %d\n", msgq);

    struct my_msgbuf msg;
    memset(&msg, 0, sizeof(msg));
    msg.mtype = 1;
    msg.pid = getpid();
    msg.io_time = 10;

    // Send the message
    ret = msgsnd(msgq, &msg, sizeof(msg) - sizeof(long), 0);
    if (ret == -1) {
        perror("msgsnd failed");
        return 1;
    }
    printf("msgsnd ret: %d\n", ret);

    return 0;
}
