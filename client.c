#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SEM_KEY 2017
#define SHM_KEY 2017


int main() {

	int shm_id, sem_id;
	char * shm_buf;
	struct sembuf sb[1];
	shm_id = shmget (SHM_KEY, 1, 0600);
	if (shm_id == -1) {
		fprintf (stderr, "shmget() error\n");
		return 1;
	}
	printf("shm_id %d\n", shm_id);
	sem_id = semget (SEM_KEY, 1, 0600);
	if (sem_id == -1) {
		fprintf (stderr, "semget() error\n");
		return 1;
	}
	shm_buf = (char *) shmat (shm_id, 0, 0);
	if (shm_buf == (char *) -1) {
		fprintf (stderr, "shmat() error\n");
		return 1;
	}
	printf ("Message: %s\n", shm_buf);
	sb[0].sem_num = 0;
	sb[0].sem_flg = SEM_UNDO;
	sb[0].sem_op = 1;
	semop (sem_id, sb, 1);
	shmdt (shm_buf);
	/*
	client code
	*/
	return 0;
}