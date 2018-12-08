#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include "common.h"

// 由于有名信号量的值是保存在文件中的，所以对于相关进程来说，
// 子进程是继承了父进程的文件描述符，那么子进程所继承的文件描述符所指向的文件是和父进程一样的，
// 当然文件里面保存的有名信号量值就共享了。
sem_t *sem_send;
sem_t *sem_recive;

int main()
{
    char msg[512];    
    int shmid = shmget(key, sizeof(message), IPC_CREAT | 0666);
    check_error(shmid);
    void *shmp = shmat(shmid, NULL, 0);
    sem_send = sem_open(name, O_CREAT, 0666, 1);
    sem_recive = sem_open(name, O_CREAT, 0666, 1);
    int i; //see the value of sem_recive
    if(sem_getvalue(sem_recive,&i)==0){
		while(i>2)
			sem_wait(sem_recive);
		if(i==0)
			sem_post(sem_recive);
	}
    while(1){
    	sem_wait(sem_recive);
	sleep(3);
    	strcpy(msg, ((message*)shmp)->text);
	if(strcmp(msg,"exit ")==0){
		strcpy(((message*)shmp)->text,"over");
		break;
	}else{
		if(strcmp(msg,"")!=0){
			printf("接受的消息: %s\n", msg);
			printf("len=%d\n",strlen(msg));	
			if(strlen(msg)==MAX_SIZE){
				printf("buf is full");
				sem_post(sem_send);			
			}
			strcpy(((message*)shmp)->text,"");
		}
	}
	sem_post(sem_recive); 
	
    }

    sem_close(sem_send);

    printf("receiver end\n");
    return 0;
}
