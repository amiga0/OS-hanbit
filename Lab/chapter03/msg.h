#ifndef MSG_H
#define MSG_H

#include <sys/types.h> // For pid_t

// Message buffer structure for inter-process communication
struct my_msgbuf {
    long mtype;    // Message type (must be > 0)

    pid_t pid;     // Process ID of the sender
    int io_time;   // Time (in milliseconds) the process will sleep
};

#endif // MSG_H
