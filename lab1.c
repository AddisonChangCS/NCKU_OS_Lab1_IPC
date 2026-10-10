#include "lab1.h"

/*    1. Use flag to determine the communication method
      2. According to the communication method, receive the message*/
void receive(message_t *message_ptr, mailbox_t *mailbox_ptr) {
  if (mailbox_ptr->flag==MSG_PASSING)
  {
    msgrcv(mailbox_ptr->storage.msqid, message_ptr, sizeof(message_t)-sizeof(long), 0, 0);
  }
  else if (mailbox_ptr->flag==SHARED_MEM)
  {
    memcpy(message_ptr, mailbox_ptr->storage.shm_addr, sizeof(message_t));
  }
  
}

/*    1. Use flag to determine the communication method
      2. According to the communication method, send the message*/
void send(message_t message, mailbox_t *mailbox_ptr) {
  if (mailbox_ptr->flag==MSG_PASSING)
  {
    msgsnd(mailbox_ptr->storage.msqid, &message, sizeof(message_t)-sizeof(long), 0);
  }
  else if (mailbox_ptr->flag==SHARED_MEM)
  {
    memcpy(mailbox_ptr->storage.shm_addr, &message, sizeof(message_t));
  }
}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    printf("Usage: %s sender <mechanism> input_file \n", argv[0]);
    printf("       %s receiver <mechanism> \n", argv[0]);
    return EXIT_FAILURE;
  }
  
  message_t message={
    .mType=1
  };
  mailbox_t mailbox={
    .flag=atoi(argv[2]),
    .sender=sem_open("/sender", O_CREAT, 0644, 1),
    .receiver=sem_open("/receiver", O_CREAT, 0644, 0)
  };
  double taken_time=0;
  struct timespec start, end;
  int shmid=-1;

  //mailbox attach
  if (mailbox.flag==SHARED_MEM)
  {
    shmid=shmget(KEY, sizeof(message_t), IPC_CREAT | 0666);
    mailbox.storage.shm_addr=shmat(shmid, NULL, 0);
  }
  else if (mailbox.flag==MSG_PASSING)
  {
    mailbox.storage.msqid=msgget(KEY, IPC_CREAT | 0666);
  }
  
  //core logic
  if (strcmp(argv[1], "sender")==0)
  {
    FILE* fp=fopen(argv[3], "r");
    while (fgets(message.msgText, MAX_MSG_SIZE, fp) != NULL)
    {
      sem_wait(mailbox.sender);

      clock_gettime(CLOCK_MONOTONIC, &start);
      send(message, &mailbox);
      clock_gettime(CLOCK_MONOTONIC, &end);

      taken_time+=((end.tv_sec - start.tv_sec) + (end.tv_nsec-start.tv_nsec)*1e-9);
      printf("Sent: %s", message.msgText);

      sem_post(mailbox.receiver);
    }
    //ending msg sending
    sem_wait(mailbox.sender);
    strcpy(message.msgText, "EXIT");
    clock_gettime(CLOCK_MONOTONIC, &start);
    send(message, &mailbox);
    clock_gettime(CLOCK_MONOTONIC, &end);

    taken_time+=((end.tv_sec - start.tv_sec) + (end.tv_nsec-start.tv_nsec)*1e-9);
    sem_post(mailbox.receiver);
    //time print
    printf("Total time in sending message: %f\n", taken_time);    
  }
  else if (strcmp(argv[1], "receiver")==0)
  {
    while (1)
    {
      sem_wait(mailbox.receiver);

      clock_gettime(CLOCK_MONOTONIC, &start);
      receive(&message, &mailbox);
      clock_gettime(CLOCK_MONOTONIC, &end);

      taken_time+=((end.tv_sec - start.tv_sec) + (end.tv_nsec-start.tv_nsec)*1e-9);
      if (strcmp(message.msgText, "EXIT")==0) break;
      printf("Receive: %s", message.msgText);
      
      sem_post(mailbox.sender);
    }
    //time print
    printf("Total time in receiving message: %f\n", taken_time); 
    //semaphore cleaning
    sem_unlink("/sender");
    sem_unlink("/receiver");
    //mailbox cleaning
    if (mailbox.flag==SHARED_MEM)
    {
      shmctl(shmid, IPC_RMID, NULL);
    }
    else if (mailbox.flag==MSG_PASSING)
    {
      msgctl(mailbox.storage.msqid, IPC_RMID, NULL);
    }
  }
  return 0;
}
