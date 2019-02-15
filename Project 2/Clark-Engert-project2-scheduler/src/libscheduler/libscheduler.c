/**	@file libscheduler.c
	Author: Levi Clark and Samuel Engert
	Class: EECS 678



 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements.
*/



typedef struct _job_t{
        int arriveTime;
        int priority;
        int respTime;
        int pid;
        int lastUsedTime;
        int firstProcessTime;
        int processTime;


} job_t;




//global priorityQueue
priqueue_t q;


int numOfJobs;
int *activeCore;
int Cores;
float waitTime;
float respondTime;
float turnaroundTime;


job_t **coreArr;
scheme_t type;




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
  Initalizes the scheduler.

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
	Cores = cores;
	coreArr = malloc(cores * sizeof(job_t));

	//Average Time Tracking Varialbles/////////////
	waitTime = 0.0;
	respondTime = 0.0;
	turnaroundTime = 0.0;
	numOfJobs = 0;
	///////////////////////////////////////////////


	//initialize core arr[]
	int counter = 0;
	while(counter < cores){

		coreArr[counter] = NULL;
		counter++;
	}

	type = scheme;

	switch(type){
		case FCFS:
			priqueue_init(&q, compare_fcfs);
			break;
		case SJF:
			priqueue_init(&q, compare_sjf);
			break;
		case PSJF:
			priqueue_init(&q, compare_psjf);
			break;
		case PPRI:
			priqueue_init(&q, compare_ppri);
			break;
		case PRI:
			priqueue_init(&q, compare_pri);
			break;
		case RR:
			priqueue_init(&q, compare_rr);
			break;
		default:
			//set default to FCFS
			priqueue_init(&q, compare_fcfs);
			break;

	}

}

//This method helps find the first idle core to help with the first step of
//scheduler_new_job, it also helps clean up scheduler_new_job() by performing this outside
int findIdleCore(){
	for(int i = 0; i < Cores; i++){
		if(coreArr[i] == NULL){
			return i;//first idle core
		}
	}
	return -1; //no idle cores

}

/**
  Called when a new job arrives.

  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.

 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	int idleCore;

	job_t *temp = malloc(sizeof(job_t));

	//set all method parameters
	temp->pid = job_number;
	temp->arriveTime = time;
	temp->firstProcessTime = running_time;
	temp->priority = priority;

	temp->respTime = -1; //initialize at -1

	idleCore = findIdleCore();

	//if we found an idle core
	if(-1 != idleCore){
		coreArr[idleCore] = temp;

		float idleResponseTime = time - coreArr[idleCore]->arriveTime;
		coreArr[idleCore]->respTime = idleResponseTime;

		if(type == PSJF){
			temp->lastUsedTime = time;

		}
		return idleCore;

	}
	//use with ppri (preemptive pri)
	else if(type == PPRI){

		int i, lowPri = coreArr[0]->priority, lowCorePri = 0;

		//We will find Lowest Priority and The Core it's running on

		//iterate through cores
		while(i < Cores){
			//check if priority is greater than lowest priority so far
			if(coreArr[i]->priority > lowPri){
				lowPri = coreArr[i]->priority;
				lowCorePri = i;

			}
			//if they have the same priority, check for arrival times
			//TODO clean up these parameters, they're uuuuugggly
			else if(coreArr[i]->priority == (coreArr[lowCorePri]->arriveTime < lowPri && coreArr[i]->arriveTime )){
				lowCorePri = i;

			}

			i++;
		}

		//Compare to new job
		if(temp->priority < lowPri){

			//resets response time
			int tempResTime = time - coreArr[lowCorePri]->arriveTime;
			if(coreArr[lowCorePri]->respTime == tempResTime ){
				coreArr[lowCorePri]->respTime = -1;
			}

			//send to priority queue and replace with temp
			priqueue_offer(&q, coreArr[lowCorePri]);
			coreArr[lowCorePri] = temp;

			if(-1 == coreArr[lowCorePri]->respTime){
				//was setting = to tempResTime without realizing that will give me incorrect value at this point
				coreArr[lowCorePri]->respTime = time - coreArr[lowCorePri]->arriveTime;

			}
			return lowCorePri;
		}


	return -1;
	}
	//use with psjf (preemptive sfj)
	else if(type == PSJF){
		int longestTimeTemp = -1;
		int spotOfLongestTimeTemp;

		for(int i = 0; i < Cores; i++){
			int tempTime = time -coreArr[i]->lastUsedTime;
			coreArr[i]->processTime = coreArr[i]->processTime -tempTime;
			coreArr[i]->lastUsedTime = time;

			//update longest time temp with our new found longest time
			if(longestTimeTemp < coreArr[i]->processTime){
				longestTimeTemp = coreArr[i]->processTime;
				spotOfLongestTimeTemp = i; //update index
			}
		}


		if(running_time < longestTimeTemp){

			//made this variable just to try and clean up if statement a bit below
			int tempTime2 = time - coreArr[spotOfLongestTimeTemp]->arriveTime;

			if(coreArr[spotOfLongestTimeTemp]->respTime == tempTime2){
				//reset response Time
				coreArr[spotOfLongestTimeTemp]->respTime = -1;
			}

			priqueue_offer(&q, coreArr[spotOfLongestTimeTemp]);

			coreArr[spotOfLongestTimeTemp] = temp; //set back to temp

			if(-1 == coreArr[spotOfLongestTimeTemp]->respTime){
				coreArr[spotOfLongestTimeTemp]->respTime = time - coreArr[spotOfLongestTimeTemp]->arriveTime;

			}
			//return the index
			return spotOfLongestTimeTemp;
		}

	}

	priqueue_offer(&q, temp);


	return -1;
}


/**
  Called when a job has completed execution.

  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.

  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{



	//Average Time Tracking Varialbles/////////////
	waitTime += time -coreArr[core_id]->arriveTime - coreArr[core_id]->firstProcessTime;
	turnaroundTime += time - coreArr[core_id]->arriveTime;

	respondTime += coreArr[core_id]->respTime;

	numOfJobs++;
	///////////////////////////////////////////////


	//Free core
	free(coreArr[core_id]);
	coreArr[core_id] = NULL;

	if(priqueue_size(&q) != 0){
		job_t *tempJob = (job_t*)priqueue_poll(&q);

		if(type == PSJF){
			tempJob->lastUsedTime = time;
		}

		coreArr[core_id] = tempJob;
		int tempTime3 = time - coreArr[core_id]->arriveTime;
		if(coreArr[core_id]->respTime == -1){
			coreArr[core_id]->respTime = tempTime3;
		}
		return tempJob->pid;
	}




	return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.

  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	job_t* currJob = coreArr[core_id];

	//check if empty
	if(currJob == NULL){
		if(priqueue_size(&q) == 0){
			return -1;
		}

	}
	//else, it isnt empty, push to priority queue
	else{
		priqueue_offer(&q, currJob);
	}

	coreArr[core_id] = priqueue_poll(&q);

	if(coreArr[core_id]->respTime == -1){
		coreArr[core_id]->respTime = time - coreArr[core_id]->arriveTime;

	}

	return coreArr[core_id]->pid;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	if(waitTime != 0.0){
		float avgWaitingTime = (float)waitTime / numOfJobs;
		return avgWaitingTime;

	}
	else{
		return 0.0;
	}

}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{

	if(turnaroundTime != 0.0){
		float avgTurnaroundTime = (float)turnaroundTime / numOfJobs;
		return avgTurnaroundTime;

	}
	else{
		return 0.0;
	}

}

/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	if(respondTime != 0.0){
		float avgResponseTime = (float)respondTime / numOfJobs;
		return avgResponseTime;
	}
	else{
		return 0.0;
	}
}


/**
  Free any memory associated with your scheduler.

  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
	for(int i = 0; i< Cores; i++){
		//if is not empty, free it
		if(coreArr[i] != NULL){
			free(coreArr[i]);

		}

	}
	free(coreArr);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)

  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
	return;
	//priqueue_print(&q);
}


/////////////////////////////////////////////////////////////////////////////
/////////// COMPARE FUNCTIONS ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


int compare_fcfs(const void* a, const void* b){
	return -1;


}

int compare_sjf(const void* a, const void* b){//runs based on shortest job first

	job_t job1 = *((job_t*)a);
	job_t job2 = *((job_t*)b);
	return((job1.processTime) - (job2.processTime));
}
// Use compare_sjf() for this .
int compare_psjf(const void* a, const void* b){
	job_t job1 = *((job_t*)a);
	job_t job2 = *((job_t*)b);

	return((job1.processTime) - (job2.processTime));

}

//Round Robin Comparison Ftn.
int compare_rr(const void* a, const void* b){//run each for quantum time
	return -1;



}

//Priority based
int compare_pri(const void* a, const void* b){
	job_t job1 = *((job_t*)a);
	job_t job2 = *((job_t*)b);

	int temp = job1.priority - job2.priority;

	if(temp == 0){
		return job1.arriveTime - job2.arriveTime;

	}
	else{
		return temp;
	}

}

// Use compare_pri() for this as well
int compare_ppri(const void* a, const void* b){
	job_t job1 = *((job_t*)a);
	job_t job2 = *((job_t*)b);

	int temp = job1.priority - job2.priority;

	if(temp == 0){
		return job1.arriveTime - job2.arriveTime;

	}
	else{
		return temp;
	}
}
