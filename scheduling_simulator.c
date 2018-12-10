#include "scheduling_simulator.h"
static ucontext_t start;
static ucontext_t end;
static ucontext_t shell_mode;
static ucontext_t current;
int PID=0;

void hw_suspend(int msec_10)
{
	set_timer(0);
	strcpy(now->state,"TASK_WAITING");
	now->Suspend_time = msec_10*10;
	Node* tmp = now;
	now = NULL;
	swapcontext(&tmp->task, &start);
	return;
}

void hw_wakeup_pid(int pid)
{
	Node* tmpnode;
	tmpnode = front->next;
	while(tmpnode != NULL && strcmp(tmpnode->state,"TASK_WAITING")==0) {
		if(tmpnode->pid == pid)
			add_ready_q(tmpnode);
		tmpnode = tmpnode ->next;
	}
	return;
}

int hw_wakeup_taskname(char *task_name)
{
	Node* tmpnode;
	int wake_num = 0;
	tmpnode = front->next;
	while(tmpnode != NULL) {
		if(strcmp(tmpnode->name,task_name)==0 && strcmp(tmpnode->state,"TASK_WAITING")==0) {
			add_ready_q(tmpnode);
			wake_num++;
		}
		tmpnode = tmpnode ->next;
	}
	return wake_num;
}

int hw_task_create(char *task_name)
{
	if(strcmp(task_name,"task1")==0 || strcmp(task_name,"task2")==0 || strcmp(task_name,"task3")==0 || 
	strcmp(task_name,"task4")==0 || strcmp(task_name,"task5")==0 || strcmp(task_name,"task6")==0){
		add(task_name,10,0,"L","S");
		return PID; // the pid of created task name
	} else	return -1;
}

void add(char* name, int Q_time, int pri, char* priority, char* quantum)
{
	Node *newnode;
	newnode = (Node*)malloc(sizeof(Node));
	if(front->next == NULL) {
		front->next = newnode;
	}
	newnode->pid = ++PID;
	newnode->Quantum_time = Q_time;
	newnode->Qeueing_time = 0;
	newnode->pri=pri;
    	strcpy(newnode->name,name);
	strcpy(newnode->priority,priority);
	strcpy(newnode->quantum,quantum);
    	strcpy(newnode->state, "TASK_READY");

	char *stack[9999];
	getcontext(&newnode->task);
	newnode->task.uc_stack.ss_sp = stack;
	newnode->task.uc_stack.ss_size = 9999;
	newnode->task.uc_stack.ss_flags = 0;
	newnode->task.uc_link = &end;

    	if(strcmp(name,"task1")==0)     makecontext(&newnode->task,task1,0);
    	else if(strcmp(name,"task2")==0)     makecontext(&newnode->task,task2,0);
    	else if(strcmp(name,"task3")==0)     makecontext(&newnode->task,task3,0);
    	else if(strcmp(name,"task4")==0)     makecontext(&newnode->task,task4,0);
    	else if(strcmp(name,"task5")==0)     makecontext(&newnode->task,task5,0);
    	else if(strcmp(name,"task6")==0)     makecontext(&newnode->task,task6,0);
	
	newnode->next = NULL;
	rear->next = newnode;
	rear = newnode;
	add_ready_q(newnode);
}

void add_ready_q(Node* newnode)
{
	newnode->lnext = NULL;
	if(lfront == lrear) {
		lfront->lnext = newnode;
		lrear = newnode;
	} else {
		lrear->lnext = newnode;
		lrear=newnode;
	}
	strcpy(newnode->state,"TASK_READY");
	//newnode->S_time = getCurrentTime();
	newnode->Start_time = 0;
}

Node* del_ready_q()
{
	Node* delnode = lfront->lnext;
	if(delnode==NULL)
		return NULL;
	if(lfront->lnext == lrear) {
		lrear = lfront;
		lfront->lnext = NULL;
	} else {
		lfront->lnext = delnode->lnext;
	}
	delnode->lnext = NULL;
	//renew Queueing time
	//delnode->W_time +=(getCurrentTime()-delnode->S_time);
	delnode->Qeueing_time += 10;
	return delnode;
}
void rm_ready_q(int pid)
{
	Node* rmnode;
	Node* tmpnode;
	if(lfront->lnext == NULL)
		return;
	tmpnode = lfront;
	while(tmpnode->lnext != NULL) {
		rmnode = tmpnode->lnext;
		if(rmnode->pid == pid && lfront->lnext->lnext == NULL) {
			lfront->lnext=NULL;
			lrear = lfront;
			return;
		} else if(rmnode->pid == pid) {
			tmpnode->lnext = rmnode->lnext;
			rmnode->lnext = NULL;
			return;
		}
		tmpnode = tmpnode->lnext;
	}
}
void removeTask(int pid)
{
	Node* rmnode;
	Node* tmpnode;
	rm_ready_q(pid);
	if(front->next == NULL) {
		return;
	}
	tmpnode = front;
	while(tmpnode->next != NULL) {
		rmnode = tmpnode->next;
		if(rmnode->pid == pid && front->next->next == NULL) {
			front->next=NULL;
			rear = front;
			return;
		} else if(rmnode->pid == pid) {
			tmpnode->next = rmnode->next;
			rmnode->next = NULL;
			return;
		}
		tmpnode = tmpnode->next;
	}
}

void showInfo()
{
	Node* tmpnode;
	tmpnode = front->next;
	printf("PID TaskName TaskState       QueingTime Priority TimeQuantum\n");
	while(tmpnode != NULL) {
		//if(strcmp(tmpnode->state,"TASK_WAITING")==0) tmpnode->W_time -= 10;
		printf("%-4d%-9s%-16s%-11d%-9s%s\n",tmpnode->pid,tmpnode->name,tmpnode->state,tmpnode->Qeueing_time,tmpnode->priority,tmpnode->quantum);
		tmpnode = tmpnode->next;
	}
}

void shellMode()
{
	char command[10];
        char TASK_NAME[10];
        char tmp[100];
        char argument1[5];
        char TIME_QUANTUM[5];
        char argument2[5];
        char PRIORITY[5];
        int qTime, pri, PID;

        while(1){
                printf("$");
                scanf("%s",command);
                if(strcmp(command,"add")==0){
                        scanf("%s",TASK_NAME);
                        fgets(tmp,sizeof(tmp),stdin);
                        memset(argument1,0,5);
                        memset(TIME_QUANTUM,0,5);
                        memset(argument2,0,5);
                        memset(PRIORITY,0,5);
                        sscanf(tmp,"%s%s%s%s",argument1, TIME_QUANTUM, argument2, PRIORITY);

                        if(strcmp(argument1,"-t")==0){
                                if(strcmp(TIME_QUANTUM,"L")==0)	qTime=20;
                                else if(strcmp(TIME_QUANTUM,"S")==0)	qTime=10;
                                if(strcmp(argument2,"-p")==0){
                                        if(strcmp(PRIORITY,"H")==0)     pri=1;
                                        else if(strcmp(PRIORITY,"L")==0)     pri=0;
                                }
                                else{
					PRIORITY[0]='L';
					pri=0;
				}
                        }
                        else{
				TIME_QUANTUM[0]='S';
                                qTime=10;
				PRIORITY[0]='L';
                                pri=0;
                        }
                        add(TASK_NAME, qTime, pri, PRIORITY, TIME_QUANTUM);
                }
                else if(strcmp(command,"remove")==0){
                        scanf("%d",&PID);
                        removeTask(PID);
                }
                else if(strcmp(command,"start")==0){
			printf("simulating...\n");
                        swapcontext(&shell_mode,&start);
                }
                else if(strcmp(command,"ps")==0){
                        showInfo();
                }
        }
}

int count=0;
void handler(int sig)
{
	//RR(every 10ms)
	if(sig==SIGALRM){
		if(now == NULL)	setcontext(&start);
		Node* tmpnode;
		tmpnode = front->next;
		while(tmpnode != NULL) {
			//printf("while pid: %d, W_time: %lld\n",tmpnode->pid,tmpnode->W_time);
			if(!strcmp(tmpnode->state,"TASK_WAITING")) {
				tmpnode->Suspend_time -= 10;//now->Quantum_time;
				tmpnode->Qeueing_time +=  10;
				//printf("if pid: %d, W_time: %lld\n",tmpnode->pid,tmpnode->W_time);
				//wake up task
				if(tmpnode->Suspend_time <= 0){
					//printf("into handler waiting\n");
					add_ready_q(tmpnode);
				}
			}
			tmpnode = tmpnode->next;
		}

		add_ready_q(now);
		Node* run_node = del_ready_q();
		strcpy(run_node->state,"TASK_RUNNING");
		now = run_node;
		run_node->Quantum_time -= 10 ;
		if(now->Quantum_time <= 0 && count==0) {
			count=1;
			//strcpy(now->state,"TASK_TERMINATED");Node* exitt;exitt=now;printf("exitt: %d\n",exitt->pid);
			rm_ready_q(now->pid);
			set_timer(run_node->Quantum_time);
			swapcontext(&lrear->task,&end);			
		}
		else {
			//printf("PID: %d\n",now->pid);
			//printf("now->Q_time: %d\n",now->Q_time);
			set_timer(run_node->Quantum_time);
			swapcontext(&lrear->task,&now->task);
		}
	}
	//ctrl+Z
	else if(sig==SIGTSTP){
		printf("\n");
		if(now!=NULL) {
			add_ready_q(now);
			now = NULL;
			set_timer(0);
		}
		getcontext(&current);
		swapcontext(&current,&shell_mode);
	}
}

void set_timer(int Q_time)
{
	new_value.it_value.tv_sec=0;
	new_value.it_value.tv_usec=Q_time*1000;//2000000;
	new_value.it_interval.tv_sec=0;
	new_value.it_interval.tv_usec=0;
	setitimer(ITIMER_REAL, &new_value, &old_value);
}

bool task_exist()
{
	Node* tmpnode;
	tmpnode = front->next;
	while(tmpnode != NULL) {
		if(strcmp(tmpnode->state,"TASK_TERMINATED"))
			return 1;
		tmpnode = tmpnode ->next;
	}
	return 0;
}

void termination()
{
	while(1) {
		set_timer(0);
		if(now == NULL) return;
		strcpy(now->state,"TASK_TERMINATED");
		now = NULL;
		swapcontext(&end,&start);
	}
}

void simulation()
{
	while(1) {
		if(!task_exist())
			swapcontext(&start,&shell_mode);
		
		Node* run_node = del_ready_q();
		//has task waiting
		if(run_node == NULL) {  
			usleep(10000000);
			Node* tmpnode;
			tmpnode = front->next;
			while(tmpnode != NULL) {
				if(strcmp(tmpnode->state,"TASK_WAITING")==0) {
					tmpnode->Suspend_time-=10;
					tmpnode->Qeueing_time +=  10;
					if(tmpnode->Suspend_time <= 0){
						//printf("into simulation waiting\n");
						add_ready_q(tmpnode);
					}
				}
				tmpnode = tmpnode ->next;
			}
		} else {
			now = run_node;
			strcpy(run_node->state,"TASK_RUNNING");
			set_timer(run_node->Quantum_time);
			swapcontext(&start,&run_node->task);
		}
	}
}

int main()
{
	//signal handler
	struct sigaction act;
	act.sa_handler = handler;			//call handler function
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGALRM);
	sigaddset(&act.sa_mask, SIGTSTP);
	act.sa_flags = 0;
	sigaction(SIGALRM, &act, NULL);
	sigaction(SIGTSTP, &act, NULL);

	//creat queue
	front = rear = (Node*)malloc(sizeof(Node));
	front->next = rear->next = NULL;
	lfront = lrear = (Node*)malloc(sizeof(Node));
	lfront->lnext = lrear->lnext = NULL;

	char *stack[9999];
	getcontext(&end);
	end.uc_stack.ss_sp = stack;
	end.uc_stack.ss_size = 9999;
	end.uc_stack.ss_flags = 0;
	end.uc_link = NULL;
	makecontext(&end,termination,0);

	char *stack2[9999];
	getcontext(&start);
	start.uc_stack.ss_sp = stack2;
	start.uc_stack.ss_size = 9999;
	start.uc_stack.ss_flags = 0;
	start.uc_link = NULL;
    	makecontext(&start,simulation,0);

	shellMode();

	return 0;
}
