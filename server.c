#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
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
	if ((sem_id = semget(SEM_ID, 1, PERMS | IPC_CREAT)) < 0)
		sys_err("server: can not create semaphore");
	if ((shm_id = shmget(SHM_ID, sizeof(message_t), PERMS | IPC_CREAT)) < 0)
		sys_err("server: can not create shared memory segment");
	if ((msg_p = (message_t *) shmat(shm_id, 0, 0)) == NULL)
		sys_err("server: shared memory attach error");

	semctl(sem_id, 0, SETVAL, 0);
	msg_p->type = MSG_TYPE_EMPTY;
	msg_p->count = 0;

	while (1) {
		if (msg_p->count > count_message) {
			if (semctl(sem_id, 0, GETVAL, 0))
				continue;
			semctl(sem_id, 0, SETVAL, 1);
			if (msg_p->type == MSG_TYPE_STRING)
				printf("%s\n", msg_p->string);
			if (msg_p->type == MSG_TYPE_FINISH) {
				printf("exit");
				break;
			}
			count_message = msg_p->count;
			semctl(sem_id, 0, SETVAL, 0);
		}
	}

	if (semctl(sem_id, 0, IPC_RMID, (struct semid_ds *) 0) < 0)
		sys_err("server: semaphore remove error");
	shmdt(msg_p);
	if (shmctl(shm_id, IPC_RMID, (struct semid_ds *) 0) < 0)
		sys_err("server: shared memory remove error");
	exit(0);
}
