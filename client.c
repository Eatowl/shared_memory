#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "message.h"

void sys_err(char *msg) {
	puts(msg);
	exit(1);
}

int main() {
	int shm_id, sem_id;
	int count_message = 0;
	char s[MAX_STRING];
	message_t *msg_p;
	
	if ((sem_id = semget(SEM_ID, 1, 0)) < 0)
		sys_err("server: can not get semaphore");
	if ((shm_id = shmget(SHM_ID, sizeof(message_t), 0)) < 0)
		sys_err("server: can not get shared memory segment");
	if ((msg_p = (message_t *) shmat(shm_id, 0, 0)) == NULL)
		sys_err("server: shared memory attach error");

	pid_t pid;
	if(!(pid = fork())) {
		while (1) {
			scanf("%s", s);
			if (semctl(sem_id, 0, SETVAL, 0))
				continue;
			semctl(sem_id, 0, SETVAL, 1);
			//printf("semaphore lock - write\n");
			if (strcmp(s, "exit") != 0) {
				msg_p->type = MSG_TYPE_STRING;
				++msg_p->count;
				//printf("%d\n", msg_p->count);
				strncpy(msg_p->string, s, MAX_STRING);
			} else {
				printf("stop\n");
				++msg_p->count;
				strncpy(msg_p->string, s, MAX_STRING);
				msg_p->type = MSG_TYPE_FINISH;
				//break;
			}
			semctl(sem_id, 0, SETVAL, 0);
		}
		return 0;
	}
	char story[MAX_STRING];
	while (1) {
		if (msg_p->count > count_message) {
			if (semctl(sem_id, 0, GETVAL, 0))
				continue;
			semctl(sem_id, 0, SETVAL, 1);
			//printf("semaphore lock - read\n");
			if (msg_p->type == MSG_TYPE_STRING)
				printf("%s\n", msg_p->string);
			if (strcmp(msg_p->string, "exit") == 0)
				break;
			if (msg_p->type == MSG_TYPE_FINISH) {
				printf("exit\n");
				break;
			}
			//msg_p->type = MSG_TYPE_EMPTY;
			count_message = msg_p->count;
			semctl(sem_id, 0, SETVAL, 0);
			//printf("semaphore unlock - read\n");
		}
	}
	shmdt(msg_p);
	exit(0);
}
