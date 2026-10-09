#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>

#define MSG_PASSING 1
#define SHARED_MEM 2

#define KEY 0

#define MAX_MSG_SIZE 1024

//for System V API
typedef struct {
    int flag;               // 1 for message passing, 2 for shared memory
    union{
        int msqid;          // for message passing(message queue) 
        void* shm_addr;     // for share memory
    }storage;
    sem_t* sender;          // Semaphore to control Sender's execution flow
    sem_t* receiver;        // Semaphore to control Receiver's execution flow
} mailbox_t;

//Message structure for wrapper
typedef struct {
    long mType;
    char msgText[MAX_MSG_SIZE];
} message_t;

void receive(message_t* message_ptr, mailbox_t* mailbox_ptr);
void send(message_t message, mailbox_t* mailbox_ptr);
