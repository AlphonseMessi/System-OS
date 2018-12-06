#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define N 4096

struct msg_data {
  long type;
  char text[N];
};

sem_t s1, s2, s3;

int id;
void *sender1() {
  while (1) {
    sem_wait(&s1);
    printf("This is sender1!!\n");
    printf("please input :");
    char s[111];
    scanf("%s", s);
    struct msg_data tmp1;
    tmp1.type = 1;
    if (strcmp(s, "exit") == 0) {
      memcpy(tmp1.text, "end1", sizeof("end1"));
      msgsnd(id, &tmp1, sizeof(tmp1), 0); /// msgflg
      sem_post(&s2);

      sem_wait(&s1);
      msgrcv(id, &tmp1, sizeof(tmp1), 2, 0);
      if (strcmp(tmp1.text, "over1") == 0) {
        sem_post(&s2);
        break;
      }
    } else {
      memcpy(tmp1.text, s, sizeof(s));
      int d = msgsnd(id, &tmp1, sizeof(tmp1), 0);
      if (d == 0)
        printf("send success\n");
      else
        printf("send failed \n");
    }

    sem_post(&s2);
  }
  printf("sender over\n");
  return NULL;
}

void *sender2() {
  while (1) {
    sem_wait(&s1);
    printf("This is sender2!!\n");
    printf("please input :");
    char s[111];
    scanf("%s", s);
    struct msg_data tmp2;
    tmp2.type = 1;
    if (strcmp(s, "exit") == 0) {
      memcpy(tmp2.text, "end2", sizeof("end2"));
      msgsnd(id, &tmp2, sizeof(tmp2), 0); /// msgflg
      sem_post(&s3);

      sem_wait(&s1);
      msgrcv(id, &tmp2, sizeof(tmp2), 2, 0);
      if (strcmp(tmp2.text, "over2") == 0) {
        sem_post(&s3);
        break;
      }
    } else {
      memcpy(tmp2.text, s, sizeof(s));
      int d = msgsnd(id, &tmp2, sizeof(tmp2), 0);
      if (d == 0)
        printf("send success\n");
      else
        printf("send failed \n");
    }

    sem_post(&s3);
  }
  printf("sender2 over\n");
  return NULL;
}

void *receiver() {
  while (1) {
    sem_wait(&s2);
    sem_wait(&s3);
    char s[111];

    struct msg_data tmp1;
    struct msg_data tmp2;
    printf("receicing...\n");
    long type = 1;
    int x = msgrcv(id, &tmp1, sizeof(tmp1), type, 0);
    printf("finish ...\n");

    printf("Receive :%s\n", tmp1.text);
    if (strcmp(tmp1.text, "end1") == 0) {
      tmp1.type = 2;
      memcpy(tmp1.text, "over1", sizeof("over1"));
      msgsnd(id, &tmp1, sizeof(tmp1), 0);

      sem_post(&s1);
      break;
    }
    printf("%d\n", x);

    sem_post(&s2);

     printf("receicing...\n");
    long type = 1;
    int y = msgrcv(id, &tmp3, sizeof(tmp3), type, 0);
    printf("finish ...\n");

    printf("Receive :%s\n", tmp2.text);
    if (strcmp(tmp2.text, "end1") == 0) {
      tmp2.type = 2;
      memcpy(tmp2.text, "over1", sizeof("over1"));
      msgsnd(id, &tmp2, sizeof(tmp2), 0);

      sem_post(&s1);
      break;
    }
    printf("%d\n", x);

    sem_post(&s1);
  }
  printf("receiver over\n");
  return NULL;
}

int main() {
  sem_init(&s1, 0, 1);
  sem_init(&s2, 0, 0);
  sem_init(&s3, 0, 0);

  id = msgget(0, 0666);
  if (id < 0) {
    printf("msgqueue build error\n");
    return 0;
  }
  printf("ID=%d\n", id);
  pthread_t id1, id2,id3;
  int t1 = pthread_create(&id1, NULL, sender1, NULL);
  if (t1 < 0) {
    printf("t1 error\n");
    return 0;
  }
  int t2 = pthread_create(&id2, NULL, sender2, NULL);
  if (t2 < 0) {
    printf("t2 error\n");
    return 0;
  }
  int t3 = pthread_create(&id3, NULL, receiver, NULL);
  if (t3 < 0) {
    printf("t2 error\n");
    return 0;
  }

  pthread_join(id1, NULL);
  pthread_join(id2, NULL);
  pthread_join(id3, NULL);
  printf("main over\n");
  return 0;
}