#include "lab1.h"

void receive(message_t *message_ptr, mailbox_t *mailbox_ptr) {
  /*  TODO:
      1. Use flag to determine the communication method
      2. According to the communication method, receive the message
  */
  sem_wait(mailbox_ptr->receiver);
  if (mailbox_ptr->flag==MSG_PASSING)
  {
    msgrcv(mailbox_ptr->storage.msqid, message_ptr, sizeof(*message_ptr)-sizeof(long), 0, 0);
  }
  else if (mailbox_ptr->flag==SHARED_MEM)
  {
    memcpy(message_ptr, mailbox_ptr->storage.shm_addr, sizeof(*message_ptr));
  }
  sem_post(mailbox_ptr->sender);
}

void send(message_t message, mailbox_t *mailbox_ptr) {
  /*  TODO:
      1. Use flag to determine the communication method
      2. According to the communication method, send the message
  */
  sem_wait(mailbox_ptr->sender);
  if (mailbox_ptr->flag==MSG_PASSING)
  {
    msgsnd(mailbox_ptr->storage.msqid, &message, sizeof(message)-sizeof(long), 0);
  }
  else if (mailbox_ptr->flag==SHARED_MEM)
  {
    memcpy(mailbox_ptr->storage.shm_addr, &message, sizeof(message));
  }
  sem_post(mailbox_ptr->receiver);
}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    printf("Usage: %s sender <mechanism> input_file \n", argv[0]);
    printf("       %s receiver <mechanism> \n", argv[0]);
    return EXIT_FAILURE;
  }

  /* TODO:
    Your implementation here
  */

  return 0;
}
