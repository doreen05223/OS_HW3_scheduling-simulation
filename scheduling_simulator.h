#ifndef SCHEDULING_SIMULATOR_H
#define SCHEDULING_SIMULATOR_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ucontext.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include "task.h"

enum TASK_STATE {
	TASK_RUNNING,
	TASK_READY,
	TASK_WAITING,
	TASK_TERMINATED
};
typedef struct node {
	int pid,pri;
	char name[100];
	char state[20];
	char quantum[10];
	char priority[10];
	int Quantum_time;
	int Start_time;
	int Qeueing_time;
	int Suspend_time;
	ucontext_t task;
	struct node* next;
	struct node* lnext;
} Node;
Node* now;
Node* front, *rear;
Node* lfront, *lrear;

struct itimerval new_value,old_value;

void hw_suspend(int msec_10);
void hw_wakeup_pid(int pid);
int hw_wakeup_taskname(char *task_name);
int hw_task_create(char *task_name);
void add( char* name, int Q_time, int pri, char* quantum, char* priority);
void add_ready_q(Node* newnode);
Node* delq();
void re_ready_q(int pid);
void removeTask(int pid);
void showInfo();
void shellMode();
void handler(int sig_num);
void set_timer(int Q_time);
bool task_exist();
void start_time();
void termination();
void simulation();

#endif
