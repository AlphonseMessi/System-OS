#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define N 512
struct msqid_ds buf;
struct msg_data {
  long type;
  char text[N];
  int mw2w;
};

sem_t s1, s2, s3;

int id;


void *sender1() {
	int i;
	printf("please input 1 :\n");

	while (1) {
		char s[500];
		gets(s);
		struct msg_data tmp;
        tmp.mw2w = 1;
		tmp.type = 1;
		if(strcmp(s,"exit") == 0){
     			memcpy(tmp.text, "end1", sizeof("end1"));
			sem_wait(&s1);
     			msgsnd(id, &tmp, sizeof(tmp), 0); // msgflg
			sem_post(&s1);
			sem_wait(&s2);
			sem_wait(&s1);
			msgrcv(id, &tmp, sizeof(tmp), 2, 0);
			sem_post(&s1);
                        if (strcmp(tmp.text, "over1") == 0) {
                              break;
			}
		} 
		else {
			memcpy(tmp.text, s, strlen(s));
			tmp.text[strlen(s)] = '\0';
			sem_wait(&s1);
			int d = msgsnd(id, &tmp, sizeof(tmp), 0);
			if(d != 0)
        		printf("send1 failed \n");
  			sem_post(&s1);
    		}
  	}
  	printf("sender1 over\n");
  	return NULL;
}

void *sender2() {
	int i;
	printf("please input 2 :\n");

	while (1) {
		char s[500];
		gets(s);
		struct msg_data tmp;
        tmp.mw2w = 2;
		tmp.type = 1;
		if(strcmp(s,"exit") == 0){
     			memcpy(tmp.text, "end2", sizeof("end2"));
			sem_wait(&s1);
     			msgsnd(id, &tmp, sizeof(tmp), 0); // msgflg
			sem_post(&s1);
			sem_wait(&s2);
			sem_wait(&s1);
			msgrcv(id, &tmp, sizeof(tmp), 2, 0);
			sem_post(&s1);
                        if (strcmp(tmp.text, "over2") == 0) {
                              break;
			}
		} 
		else {
			memcpy(tmp.text, s, strlen(s));
			tmp.text[strlen(s)] = '\0';
			sem_wait(&s1);
			int d = msgsnd(id, &tmp, sizeof(tmp), 0);
			if(d != 0)
        		printf("send2 failed \n");
  			sem_post(&s1);
    		}
  	}
  	printf("sender2 over\n");
  	return NULL;
}

void *receiver() {
	while (1) {
		sleep(5);
		int i;
		int flag = 0;
    	char s[1024];
		int x;

    	struct msg_data tmp;
    	long type = 1;
		sem_wait(&s1);
		printf("get success\n");
        if(tmp.mw2w == 1)
        {
		while(x = msgrcv(id, &tmp, sizeof(tmp), type, IPC_NOWAIT) != -1) {
			sem_post(&s1);
			
			if(x!=-1&&strcmp(tmp.text,"end1")!=0){
				printf("Receive from sender1 :%s\n",tmp.text);
			}else{
    				if (strcmp(tmp.text, "end1") == 0) {
      					tmp.type = 2;
      					memcpy(tmp.text, "over1", sizeof("over1"));
					sem_wait(&s1);
					msgsnd(id, &tmp, sizeof(tmp), 0);
					sem_post(&s2);
					sem_post(&s1);
					flag = 1;
					break;
				}
 			}
		}
		if(flag == 1) break; 
        }

        if(tmp.mw2w == 2)
        {
		while(x = msgrcv(id, &tmp, sizeof(tmp), type, IPC_NOWAIT) != -1) {
			sem_post(&s1);
			
			if(x!=-1&&strcmp(tmp.text,"end2")!=0){
				printf("Receive from sender2 :%s\n",tmp.text);
			}else{
    				if (strcmp(tmp.text, "end2") == 0) {
      					tmp.type = 2;
      					memcpy(tmp.text, "over2", sizeof("over2"));
					sem_wait(&s1);
					msgsnd(id, &tmp, sizeof(tmp), 0);
					sem_post(&s2);
					sem_post(&s1);
					flag = 1;
					break;
				}
 			}
		}
		if(flag == 1) break; 
        }
	}
	printf("receiver over\n");
 	return NULL;
}

int main() {
  sem_init(&s1, 0, 1);
  sem_init(&s2, 0, 0);
  id = msgget(0, 0666);
  if (id < 0) {
    printf("msgqueue build error\n");
    return 0;
  }
  pthread_t id1, id2,id3;
  int t1 = pthread_create(&id1, NULL, sender1, NULL);
  if (t1 < 0) {
    printf("t1 error\n");
    return 0;
  }
  int t2 = pthread_create(&id2, NULL, sender2, NULL);
  if (t2 < 0) {
    printf("t1 error\n");
    return 0;
  }
  int t3 = pthread_create(&id3, NULL, receiver, NULL);
  if (t3 < 0) {
    printf("t3 error\n");
    return 0;
  }

  pthread_join(id1, NULL);
  pthread_join(id2, NULL);
  pthread_join(id3, NULL);
  printf("main over\n");
  return 0;
}

