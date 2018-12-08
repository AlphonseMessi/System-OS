#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include "common.h"

sem_t *sem_send;
sem_t *sem_recive;

int main()
{
    // init
	char msg[512];
	char msgBuf[512];
	strcpy(msg,"");
    	int shmid = shmget(key, sizeof(message), 0666 | IPC_CREAT);
	check_error(shmid);
    	void* shmp = shmat(shmid, NULL, 0);
    	sem_send = sem_open(name, O_CREAT, 0666, 1);
	sem_recive = sem_open(name, O_CREAT, 0666, 1);
	
	//这段代码的目的是防止之前使用Ctrl+C强行终止进程导致重新允许时初始化信号量时，信号量sem_send的值为之前运行的0
	int i;//see the value of sem_send
	if(sem_getvalue(sem_send,&i)==0){
		while(i>2)
			sem_wait(sem_send);
		if(i==0)
			sem_post(sem_send);
	}
    		
    while(1){
	sem_wait(sem_send);
	printf("input:");
	gets(msg);
	if(strlen(((message*)shmp)->text)+strlen(msg)>=MAX_SIZE){//当共享内存满的情况
		printf("buffer is full! please wait!\n");
		int len=MAX_SIZE-strlen(((message*)shmp)->text);//目前共享内存可用容量len，可能取值：0-512
		if(strlen(msg)>MAX_SIZE){//目的是为了分段发送消息
			int count=((strlen(msg)-len)/MAX_SIZE)+1+1;//该字段是为了计算要分段发送几次数据，两次+1的其中一个是为了取上整数，另一个是因为减去了len(一次len占用一次发送)。
			//eg：假设目前共享内存用了500，则len=12，而用户输入了长度为520的数据，此时count=3；则第一次要发送len=12的数据，
			//第二次发送512长度数据，第三次发送8长度数据。
			int index=0;//循环变量
			printf("该条消息将分%d段发送!\n",count);
			while(index<count){
				len=MAX_SIZE-strlen(((message*)shmp)->text);//这里重新计算是因为发送了一次数据被接收后，共享内存可用容量会变化
				printf("发送第%d条消息\n",index+1);
				strncat(((message*)shmp)->text,msg,len);
				strncpy(msg,msg+len,512);//把msg前len的数据截取弃用掉，因为发送一次后要发送的数据长度会变短len。
				index++;
				if(index<count){//最后一次发送之后已经不需要再阻塞自己了
					sem_wait(sem_send);
				}else sleep(1);
			}
		}else{
			strncat(((message*)shmp)->text,msg,len);
			sem_wait(sem_send);
		}
		
	}else{
		strcat(((message*)shmp)->text,msg);
		strcat(((message*)shmp)->text," ");
		printf("msg:%s,len:%d\n",((message*)shmp)->text,strlen(((message*)shmp)->text));		
	}
	
	

	strcpy(msg, ((message*)shmp)->text);
	if(strcmp(msg,"over")==0){
		printf("sender receive:over\n");
		sem_close(sem_send);
		break;
	}	
	sem_post(sem_send);
    }

	sem_unlink(name);

    	shmctl(shmid, IPC_RMID, shmp);
    	printf("sender end\n");
    	return 0;
}
