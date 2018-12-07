#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>


\

void *sender1(void *arg);
void *sender2(void *arg);
void *receiver(void *arg);
void P(sem_t *sem_ptr);
void V(sem_t *sem_ptr);

sem_t send_psx, recv_psx, final_recv_1, final_recv_2;	
pthread_t send_pid_1, send_pid_2, recv_pid;		
int send_1_over = 0;				
int send_2_over = 0;				

struct msg_data {
  long type;
  char text[N];
  int mw2w;
};


int id;



void *sender1(void *arg)
{
    char s[500];                                     	
    struct msg_data tmp;			
    tmp.type = 1;
    tmp.mw2w = 1;
    while (1) {
        sem_wait(&send_psx);

	   
        printf("Sender1 send: ");
        scanf("%s", s);
  
        if ((strcmp(s, "exit") == 0) || (strcmp(s, "end1") == 0)) {
            strcpy(tmp.text, "end1");
            msgsnd(id, &tmp, sizeof(tmp), 0);
            sem_post(&recv_psx);
            break;
        }
        strcpy(tmp.text, s);
        
        msgsnd(id, &tmp, sizeof(tmp), 0);
        sem_post(&recv_psx);
	
    }
    sem_wait(&final_recv_1);					
    msgrcv(id, &tmp, sizeof(tmp, recv_type, 0);	
    printf("Sender1 receive: %s\n", tmp.text);
    

    sem_post(&send_psx);					
  
    if (send_1_over && send_2_over){			
	msgctl(id, IPC_RMID, 0);			
    }			
    pthread_exit(NULL);					
}


void *sender2(void *arg)
{
    char s[500];                                     	
    struct msg_data tmp;			
    tmp.type = 1;
    tmp.mw2w = 2;
    while (1) {
        sem_wait(&send_psx);

	   
        printf("Sender2 send: ");
        scanf("%s", s);
  
        if ((strcmp(s, "exit") == 0) || (strcmp(s, "end2") == 0)) {
            strcpy(tmp.text, "end2");
            msgsnd(id, &tmp, sizeof(tmp), 0);
            sem_post(&recv_psx);
            break;
        }
        strcpy(tmp.text, s);
        
        msgsnd(id, &tmp, sizeof(tmp), 0);
        sem_post(&recv_psx);
	
    }
    sem_wait(&final_recv_2);					
    msgrcv(id, &tmp, sizeof(tmp, recv_type, 0);	
    printf("Sender1 receive: %s\n", tmp.text);
    

    sem_post(&send_psx);					
  
    if (send_1_over && send_2_over){			
	msgctl(id, IPC_RMID, 0);			
    }			
    pthread_exit(NULL);					
}

void *receiver(void *arg)
{
    struct msg_data tmp;				
    while (1) {
        sem_wait(&recv_psx);
        msgrcv(msgid, &tmp, sizeof(tmp), send_type, 0);
	if (tmp.mw2w == 1){		
            if (strcmp(tmp.text, "end1") == 0) {
            	strcpy(tmp.text, "over1");
            	tmp.type = 2;
		        tmp.mw2w = 3;
            	msgsnd(id, &tmp, sizeof(tmp), 0);
            	printf("Receiver receive 'end1' from sender1, return 'over1'\n");

            	sem_post(&final_recv_1);
                send_1_over = 1;
            }
            else {
            	printf("Receiver receive: %s from Sender1\n", tmp.text);
	    	sem_post(&send_psx);
	    }
	}
	else if (tmp.mw2w == 2 {	
            if (strcmp(tmp.text, "end2") == 0) {
            	strcpy(tmp.text, "over2");
            	tmp.type = 2;
		        tmp.mw2w = 4;
            	msgsnd(id, &tmp, sizeof(tmp), 0);
            	printf("Receiver receive 'end2' from Sender2, return 'over2'\n");

            	V(&final_recv_2);
		        send_2_over = 1;
            	
            }
            else {
                printf("receiver receive: %s from Sender2\n", tmp.text);
	        V(&send_psx);
	    }
	}
	

	if (send_1_over && send_2_over){		// 2个 sender线程 都发送过 'end' 且收到过 'over' 后，将跳出循环，结束当前线程
	    break;
	}
    }
    pthread_exit(NULL);
}

int main(void)
{
    sem_init(&send_psx, 0, 1);				// pshared = 0，同一进程中多线程的同步
    sem_init(&recv_psx, 0, 0);
    sem_init(&final_recv_1, 0, 0);
    sem_init(&final_recv_2, 0, 0);

    msgid = msgget(IPC_PRIVATE, 0666|IPC_CREAT);	// 创建消息队列
    if (msgid < 0) {
        printf("[*] Error: msgget() return error\n");
        exit(1);
    }
    pthread_create(&send_pid_1, NULL, send_thread_1, NULL); // 创建 线程，并将线程加入当前 进程
    pthread_create(&send_pid_2, NULL, send_thread_2, NULL);
    pthread_create(&recv_pid, NULL, recv_thread, NULL);

    pthread_join(send_pid_1, NULL);			// 阻塞调用 send / receive 线程，功能相反于设置守护线程的 SetDaemon()
    pthread_join(send_pid_2, NULL);
    pthread_join(recv_pid, NULL);

    return 0;
}