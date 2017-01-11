#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>

#define SEM_ID 2016
#define SHM_ID 2017
#define PERMS  0666
#define MAX_STRING 120

#define MSG_TYPE_EMPTY  0
#define MSG_TYPE_STRING 1
#define MSG_TYPE_FINISH 2

typedef struct {
	int type;
	int count;
	char string[MAX_STRING];
} message_t;
