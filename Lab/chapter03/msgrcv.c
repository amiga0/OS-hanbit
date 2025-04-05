#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg.h"
#include <string.h>
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

    // Receive the message
    ret = msgrcv(msgq, &msg, sizeof(msg) - sizeof(long), 0, 0);
    if (ret == -1) {
        perror("msgrcv failed");
        return 1;
    }
    printf("msgrcv ret: %d\n", ret);
    printf("msg.mtype: %ld\n", msg.mtype);
    printf("msg.pid: %d\n", msg.pid);
    printf("msg.io_time: %d\n", msg.io_time);

    // Remove the message queue
    if (msgctl(msgq, IPC_RMID, NULL) == -1) {
        perror("msgctl failed");
        return 1;
    }

    return 0;
}
