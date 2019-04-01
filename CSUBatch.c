#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>

// Define structures for a job and for user inputJob
struct job {
	int priority;
	double arrivalTime;
	double expectedRunTime;
	double runTime;
	bool status;
    char name[100];
};

struct test {
	int schedulingPolicy;
	int numOfJobs;
	int priorityLevels;
	int minCPUtime;
	int maxCPUtime;
	char benchmark[100];
};

struct inputStruct {
	int cmd;
	struct job inputJob;
	struct test testInput;
};

struct quitVariablesStruct {
	int totalJobsSubmitted;
	double avgThroughput;
	double avgCPUTime;
	double avgWaitingTime;
	double avgTurnaroundTime;
};

// declare functions to become pthreads
void *scheduler(void *argumentsToPthread);
void *dispatcher(void *argumentsToPthread);

// declare functions
struct inputStruct userInput();
void fcfs();
void sjf();
void priority();
int compareFirstCome (const void *a, const void *b);
int compareShortestFirst (const void *a, const void *b);
int comparePriority (const void *a, const void *b);

// declare mutex
pthread_mutex_t mutex;

// Define global variables
//	jobArray to be shared by scheduler and dispatcher pthreads - acts sorta like a queue for unlaunched jobs
struct job jobArray[100];
struct job dispatchedJobs[1000];
struct quitVariablesStruct quitVariable;

int main(){

	// thread IDs
	pthread_t tid1, tid2;

	// get default pthread attributes
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	// create mutex locks
	pthread_mutex_init(&mutex,NULL);
	
	// launch scheduler and dispatcher pthreads
	pthread_create(&tid1, &attr, scheduler, NULL);
	pthread_create(&tid2, &attr, dispatcher, NULL);
	
	// code shouldn't reach past this point, stuck within while loops
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_mutex_destroy(&mutex);
	
	return 0;
}

void *scheduler(void *ptr){
	// initialize command to determine what function to run
	int command = 0;
	
	int sortingPolicy = 1;
	
	// run this for the entire program
	while(1){
		command = 0;
		struct inputStruct input = userInput();
		command = input.cmd;
		
		pthread_mutex_lock(&mutex);
		
		if (command == 1){
			// run - add job to array
			
			// add new job to first open spot in jobArray
			for(int i = 0; i < 100; i++){
				// find first open spot
				if (jobArray[i].name[0] == '\0'){
					// get the time and save it to the job's arrivalTime before adding it into the array
					struct timeval arrivalTime;
					gettimeofday(&arrivalTime, NULL);
					double timeInSec = arrivalTime.tv_sec; 
					input.inputJob.arrivalTime = timeInSec;
					// add the job to the array
					jobArray[i] = input.inputJob;
					
					quitVariable.totalJobsSubmitted++;
					break; // exit for loop
				}
			}
			
			// Re-sort
			if (sortingPolicy == 1){
				fcfs();
			} else if (sortingPolicy == 2){
				sjf();
			} else if (sortingPolicy == 3){
				priority();
			}
			
		} else if (command == 2){
			// fcfs
			sortingPolicy = 1;
			fcfs();
		} else if (command == 3){
			// sjf
			sortingPolicy = 2;
			sjf();
		} else if (command == 4){
			// priority
			sortingPolicy = 3;
			priority();
		} else if (command == 5){
			// help
			printf("run <job> <time> <pri>: submit a job named <job>, \n");
			printf("                        expected execution time is <time>, \n");
			printf("                        priority is <pri>, \n");
			printf("list: display the job status \n");
			printf("fcfs: change the scheduling policy to FCFS. \n");
			printf("sjf: change the scheduling policy to SJF. \n");
			printf("priority: change the scheduling policy to priority. \n");
			printf("test <benchmark> <policy> <num_of_jobs> <priority_levels> \n");
			printf("     <min_CPU_time> <max_CPU_time>");
			printf("quit: exit CSUbatch \n");
		} else if (command == 6){
			// list the completed jobs first, then the job being run, then the jobs in queue
			
			int totalJobsInQueue = 0;
			for (int i = 0; i < 100; i++){
				if (jobArray[i].name[0] != '\0'){
					totalJobsInQueue++;
				}
			}
			
			printf("Total number of jobs in the queue: %d \n", totalJobsInQueue);
			if (sortingPolicy == 1){
				printf("Scheduling Policy: FCFS. \n");
			} else if (sortingPolicy == 2){
				printf("Scheduling Policy: SJF. \n");
			} else if (sortingPolicy == 3){
				printf("Scheduling Policy: Priority. \n");
			}
			
			printf("Name  - CPU_Time - 	 Priority - Arrival_time -       Progress \n");
			// print out the ones that have been dispatched
			for (int i = 0; i < 1000; i++){
				if (dispatchedJobs[i].name[0] != '\0'){
					// print out the name -> arrival time
					printf("%s	%f	%d	%f	", dispatchedJobs[i].name, dispatchedJobs[i].expectedRunTime, dispatchedJobs[i].priority, dispatchedJobs[i].arrivalTime);
					// check if done or not
					if (dispatchedJobs[i].status == false){
						printf("Not finished \n");
					} else if (dispatchedJobs[i].status == true){
						printf("Finished \n");
					}
				}
			}
			for (int i = 0; i < 100; i++){
				if (jobArray[i].name[0] != '\0'){
					// print out the name -> arrival time
					printf("%s	%f	%d	%f	", jobArray[i].name, jobArray[i].expectedRunTime, jobArray[i].priority, jobArray[i].arrivalTime);
					// check if done or not
					if (jobArray[i].status == false){
						printf("Not finished \n");
					} else if (jobArray[i].status == true){
						printf("Finished \n");
					}
				}
			}
			
		} else if (command == 7){
			// test
			printf("testing is broken, sorry but could not fix all the bugs \n");
			
			// couldnt get it to work
			// have left the code within this section
			// modified/removed all other parts of code related to testing outside of here
			
			
			/*
			pthread_mutex_lock(&mutex);
			
			// set sorting policy
			// store stat tracker variables to new temp ones
			// reset stat tracker variables
			// clear job array
			// add in new set
			
			// use same job to limit other factors in testing results
			
			// use condi variable here to stall out scheduler while dispatcher processess all the new jobs
			// if no time, edit dispatcher command and dispatcher while loop to continue on test until array in clear
			
			sortingPolicy = input.testInput.schedulingPolicy;
			
			int testTotalJobsSubmitted = quitVariable.totalJobsSubmitted;
			double testAvgThroughput = quitVariable.avgThroughput;
			double testAvgCPUTime = quitVariable.avgCPUTime;
			double testAvgWaitingTime = quitVariable.avgWaitingTime;
			double testAvgTurnaroundTime = quitVariable.avgTurnaroundTime;
			
			quitVariable.totalJobsSubmitted = 0;
			quitVariable.avgThroughput = 0;
			quitVariable.avgCPUTime = 0;
			quitVariable.avgWaitingTime = 0;
			quitVariable.avgTurnaroundTime = 0;
			
			
			for(int i = 0; i < 100; i++){
				jobArray[i].priority = 0;
				jobArray[i].arrivalTime = 0;
				jobArray[i].expectedRunTime = 0;
				jobArray[i].runTime = 0;
				jobArray[i].status = false;
				
				for(int x = 0; x < 100; x++){
					jobArray[i].name[x] = '\0';
				}
			}
			
			
			//for numOfJobs, enter in processA
			//use the modulus operator in inputJob's creation to vary the priority and expected run time
			//simulate different try times but setting arrivalTime to i
			
			for (int i = 0; i < input.testInput.numOfJobs; i++){
				// make entry based off i, less that maxCPUtime
				// if ends up being too low, add one one less than minCPUtime
				int entryTime =  i % input.testInput.maxCPUtime;
				if (entryTime < input.testInput.minCPUtime){
					entryTime += input.testInput.maxCPUtime - 1;
				}
				
				struct job inputJob = {i%input.testInput.priorityLevels+1, i, entryTime, 0.0, false, '\0'};
				// was not allowed to set by = "pA", {"pA"}, or few other methods - not sure why
				inputJob.name[0] = 'p';
				inputJob.name[1] = 'B';
				jobArray[i] = inputJob;
				quitVariable.totalJobsSubmitted++;
				printf("entering a job \n");
			}
			
			// Re-sort
			if (sortingPolicy == 1){
				fcfs();
			} else if (sortingPolicy == 2){
				sjf();
			} else if (sortingPolicy == 3){
				priority();
			}
			
			dispatcherCommand = input.testInput.numOfJobs;
			
			pthread_mutex_unlock(&mutex);
			*/
			
		} else if (command == 8){
			// exit command
			// print out stats
			
			printf("Total number of jobs submitted: %d seconds \n", quitVariable.totalJobsSubmitted);
			printf("Average turnaround time:        %f seconds \n", quitVariable.avgTurnaroundTime);
			printf("Average CPU time:               %f seconds \n", quitVariable.avgCPUTime);
			printf("Average waiting time:           %f seconds \n", quitVariable.avgWaitingTime);
			printf("Throughput:                     %f No./seconds \n", quitVariable.avgThroughput);
			// exit the program - both threads included
			exit(0);
			
		}
		pthread_mutex_unlock(&mutex);
	} // end of while loop
}


void *dispatcher(void *ptr){
	// firstIndex tracks the first index that is available for a new job to be added in dispatchedJobs
	int firstIndexAvailable = 0;
	
	// stat tracker variables
	// my understanding is that turnaround time is meant to be time from first arrival in scheduler to finished running in dispatcher
	
	quitVariable.avgThroughput = 0;
	quitVariable.avgCPUTime = 0;
	quitVariable.avgWaitingTime = 0;
	quitVariable.avgTurnaroundTime = 0;
	
	while(1){
		
		pthread_mutex_lock(&mutex);
		if (jobArray[0].name[0] != '\0'){
			// set the current job to be submitted to the first empty index in our dispatched array
			dispatchedJobs[firstIndexAvailable] = jobArray[0];
			// move all jobArray entries 1 closer to 0
			for (int a = 0; a < 99; a++){
				jobArray[a] = jobArray[a+1];
			}
			// setup variables for the fork() and execv
			pid_t childPid;
			int childStatus;

			// setup execv argument
			char *processPath = (char *)malloc(strlen(dispatchedJobs[firstIndexAvailable].name)+1);
			strcpy(processPath,dispatchedJobs[firstIndexAvailable].name);

            // declare exit time here to avoid error - set value within if statement			
            double exitTime;
            
			/*
			1. calculate how much time spent between arriving in scheduler to here
			2. take time entering the fork
			3. fork
			4. in child, get the path of the job, then call execv
			5. if it failed, report probable reason why
			6. in parent, calculate run time using step #2's time and set status to done
			7. also calculate program's turnaround and throughput
			*/
			
			// calculate waiting time based on arrival time
            struct timeval waitingTimestamp;
			gettimeofday(&waitingTimestamp, NULL);
			double doneWaiting = waitingTimestamp.tv_sec;
			quitVariable.avgWaitingTime += doneWaiting - dispatchedJobs[firstIndexAvailable].arrivalTime;
            
            
            // get the time and save to firstTimestamp as milliseconds
			struct timeval firstTimestamp;
			gettimeofday(&firstTimestamp, NULL);
			double entryTime = firstTimestamp.tv_sec;
			childPid = fork();
			if(childPid == 0){
			
                char* args[] = {processPath, (char*)0};
                // redirect outputs
		        int fileOut = open("csubatch-output", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		        int saveStdout = dup(1);
		        dup2(fileOut, 1);
		        close(fileOut);
		        
		        if (execv(processPath, args) == -1){
				    /*
				    Sometimes weird bug where jobs wont launch despite having the correct path
				    Recompiling fixes the issues.
				    With no changes to the code, there shouldn't be any differences between
				    the compiled versions, yet despite that, recompiling seems to fix the issue.
				    
				    Cannot intentionally repeat the bug, but seems to happen 5% of the time
				    
				    Since it doesn't seem to be a bug caused by the code, and is instead seems to
				    
				    be a bug with compiling, we have absolutely no idea about the cause
				    */
				    
				    
		        	dup2(saveStdout, 1);
		        	close(saveStdout);
				    
				    printf("Failed to launch job - incorrect path: %s \n", processPath);
				    printf("Please check job path - if correct, then consider relaunching program");
				    printf("\n");
				}
			} else {
				// wait until child is done
                pid_t tpid;
                tpid = wait(&childStatus);
                // get the time and save the elapsed time
				struct timeval secondTimestamp;
				gettimeofday(&secondTimestamp, NULL);
				double exitTime = secondTimestamp.tv_sec;
				double elapsedTime = exitTime - entryTime;
				quitVariable.avgCPUTime += elapsedTime;
				//pthread_mutex_lock(&mutex);
				dispatchedJobs[firstIndexAvailable].runTime = elapsedTime;
				
				// add together the run time + waiting time
				quitVariable.avgTurnaroundTime += (elapsedTime + (doneWaiting - dispatchedJobs[firstIndexAvailable].arrivalTime));
				quitVariable.avgThroughput = (quitVariable.totalJobsSubmitted/quitVariable.avgTurnaroundTime);
				
				// set the job status to done
				dispatchedJobs[firstIndexAvailable].status = true;
				
				//pthread_mutex_unlock(&mutex);
				
				firstIndexAvailable++;
			}
			
			pthread_mutex_unlock(&mutex);
			
		} else {
			pthread_mutex_unlock(&mutex);
			
		} // end if statement for dispatching jobs

	} // end while loop
}


// setup userInput function
struct inputStruct userInput(){

	// array to hold the user input
	char commandInput[127] = {'\0'};
	// int to return what command (run, list, etc) the user input to the scheduler
	int command;

	printf("Enter a command \n");
	scanf(" %126[^\n]", commandInput);
	
	/*
	userInputArray holds the command
	check the first 3 letters first
	if no valid command matches up, then check 4 letters, then 8
	
	If it was a 3 letter command equal to "run", then do extra work to determine things
	like the job name, execution time, and priority.
	Assign other details that aren't decided by user input such as arrival time to be equal to 0
	*/
	char userInputArray[3] = {'\0'};
    strncpy(userInputArray, commandInput, 3);  
    userInputArray[3] = '\0';

	if (strcmp(userInputArray, "run") == 0){
		char jobName[100] = {'\0'};
		char jobExpectedRunTime[10] = {'\0'};
		char jobPriority[10] = {'\0'};
		
		int firstSpace = 0;
		int secondSpace = 0;
		char charStorage[122] = {'\0'};
		int x = 0;
		
        // reset charStorage
		for(int x = 0; x < 122; x++){
            jobName[x] = '\0';
        }
        
		// for loop the entire input array
		for (int i = 3; i < 126; i++){
			// if firstSpace has not been assigned an index
			if (firstSpace == 0){
				// find the first space then assign the index of it
				if (commandInput[i] == ' '){
					firstSpace = i;
				}
			} else if (secondSpace == 0){ 
				if (commandInput[i] == ' '){
					secondSpace = i;
				}
			}
			// when found 2 spaces
			if (firstSpace != 0 && secondSpace != 0){
				
				// take everything inbetween them and assign it to charStorage
				for (int z = firstSpace + 1; z < secondSpace; z++){
					charStorage[x] = commandInput[z];
					x++;
				}
				x = 0;
				
				// find the first variable that hasnt been assigned yet, assign it to equal charStorage
				if (jobName[0] == '\0'){
					for (int q = 0; q < 122; q++){
						if (charStorage[q] != '\0'){
							jobName[q] = charStorage[q];
						}
					}

				} else if (jobExpectedRunTime[0] == '\0'){
					for (int q = 0; q < 10; q++){
						if (charStorage[q] != '\0'){
							jobExpectedRunTime[q] = charStorage[q];
						}
					}
				} else if (jobPriority[0] == '\0'){
					for (int q = 0; q < 10; q++){
						if (charStorage[q] != '\0'){
							jobPriority[q] = charStorage[q];
						}
					}
				}
				// reset charStorage
				for(int x = 0; x < 122; x++){
					charStorage[x] = '\0';
				}
				
                // check if that was the last input
				bool finalInput = false;
				for (int i = secondSpace + 1; i < 126; i++){
					// check for non-null and non-space characters in the commandInput
					if (commandInput[i] != ' ' && commandInput[i] != '\0'){
						finalInput = true;
                        // for loop through to detect for any other spaces
                        // upon finding another space, check for a letter after it
                        // if so, not the last word
                        bool isItReallyLastWord = true;
                        for (int x = i + 1; x < 126; x++){
                            if (commandInput[x] == ' '){
                                if (commandInput[x+1] != ' ' && commandInput[x+1] != '\0'){
                                    isItReallyLastWord = false;
                                    break;
                                }
                            }
                        }
                        if (isItReallyLastWord == false){
                            finalInput = false;                     
                            break;
                        }
					}
				}
				// assign the final input
				if (finalInput == true){
					// fill the charStorage with the final input
					x = 0;
					for (int z = secondSpace + 1; z < 126; z++){
						if (commandInput[z] == ' ' || commandInput[z] == '\0'){
							break;
						}
						charStorage[x] = commandInput[z];
						x++;
					}
					// find what the final variable is meant to be
					if (jobName[0] == '\0'){
						for (int q = 0; q < 122; q++){
							if (charStorage[q] != '\0'){
								jobName[q] = charStorage[q];
							}
						}
					} else if (jobExpectedRunTime[0] == '\0'){
						for (int q = 0; q < 10; q++){
							if (charStorage[q] != '\0'){
								jobExpectedRunTime[q] = charStorage[q];
							}
						}
					} else if (jobPriority[0] == '\0'){
						for (int q = 0; q < 10; q++){
							if (charStorage[q] != '\0'){
								jobPriority[q] = charStorage[q];
							}
						}
					}
					break;
				}
				firstSpace = secondSpace;
				secondSpace = 0; 
			}
		}
		if (jobName[0] == '\0'){
			x = 0;
			secondSpace = 3;
			for (int z = secondSpace + 1; z < 126; z++){
				if (commandInput[z] == ' ' || commandInput[z] == '\0'){
					break;
				}
				charStorage[x] = commandInput[z];
				x++;
			}
			for (int q = 0; q < 122; q++){
				if (charStorage[q] != '\0'){
					jobName[q] = charStorage[q];
				}
			}
		}
		
		double expectRunTime;
		int prio;
		sscanf(jobExpectedRunTime, "%lf", &expectRunTime);
		sscanf(jobPriority, "%d", &prio);
		
		struct job newJob = {prio, 0.0, expectRunTime, 0.0, false, ""};
		strcpy(newJob.name, jobName);
		command = 1;
		struct inputStruct inputToReturn = {command, newJob, {0,0,0,0,0,'\0'}};

        // reset commandInput
		for(int x = 0; x < 127; x++){
			commandInput[x] = '\0';
        }
		return inputToReturn;

	} else if (strcmp(userInputArray, "sjf") == 0){
		command = 3;
		struct inputStruct inputToReturn = {command, {0, 0.0, 0.0, 0.0, false, '\0'}, {0,0,0,0,0,'\0'}};
		return inputToReturn;
	}
	
    char userInputArray2[4] = {'\0'};
    strncpy(userInputArray2, commandInput, 4);  
    userInputArray2[4] = '\0';
	if (strcmp(userInputArray2, "help") == 0){
		command = 5;
		struct inputStruct inputToReturn = {command, {0, 0.0, 0.0, 0.0, false, '\0'}, {0,0,0,0,0,'\0'}};
		return inputToReturn;
	} else if (strcmp(userInputArray2, "list") == 0){
		command = 6;
		struct inputStruct inputToReturn = {command, {0, 0.0, 0.0, 0.0, false, '\0'}, {0,0,0,0,0,'\0'}};
		return inputToReturn;
	} else if (strcmp(userInputArray2, "fcfs") == 0){
		command = 2;
		struct inputStruct inputToReturn = {command, {0, 0.0, 0.0, 0.0, false, '\0'}, {0,0,0,0,0,'\0'}};
		return inputToReturn;
	} else if (strcmp(userInputArray2, "test") == 0){
		command = 7;
		
		// parse statements
		char benchmarkInput[100] = {'\0'};
		char schedulingPolicyInput[10] = {'\0'};
		char numOfJobsInput[10] = {'\0'};
		char priorityLevelsInput[10] = {'\0'};
		char minCPUtimeInput[10] = {'\0'};
		char maxCPUtimeInput[10] = {'\0'};
		
		int firstSpace = 0;
		int secondSpace = 0;
		char charStorage[122] = {'\0'};
		int x = 0;
		
        // reset charStorage
		for(int x = 0; x < 122; x++){
            benchmarkInput[x] = '\0';
        }
        
		// for loop the entire input array
		for (int i = 3; i < 126; i++){
			// if firstSpace has not been assigned an index
			if (firstSpace == 0){
				// find the first space then assign the index of it
				if (commandInput[i] == ' '){
					firstSpace = i;
				}
			} else if (secondSpace == 0){ 
				if (commandInput[i] == ' '){
					secondSpace = i;
				}
			}
			// when found 2 spaces
			if (firstSpace != 0 && secondSpace != 0){
				// take everything inbetween them and assign it to charStorage
				for (int z = firstSpace + 1; z < secondSpace; z++){
					charStorage[x] = commandInput[z];
					x++;
				}
				x = 0;
				// find the first variable that hasnt been assigned yet, assign it to equal charStorage
				if (benchmarkInput[0] == '\0'){
					for (int q = 0; q < 122; q++){
						if (charStorage[q] != '\0'){
							benchmarkInput[q] = charStorage[q];
						}
					}
				} else if (schedulingPolicyInput[0] == '\0'){
					for (int q = 0; q < 10; q++){
						if (charStorage[q] != '\0'){
							schedulingPolicyInput[q] = charStorage[q];
						}
					}
				} else if (numOfJobsInput[0] == '\0'){
					for (int q = 0; q < 10; q++){
						if (charStorage[q] != '\0'){
							numOfJobsInput[q] = charStorage[q];
						}
					}
				} else if (priorityLevelsInput[0] == '\0'){
					for (int q = 0; q < 10; q++){
						if (charStorage[q] != '\0'){
							priorityLevelsInput[q] = charStorage[q];
						}
					}
				} else if (minCPUtimeInput[0] == '\0'){
					for (int q = 0; q < 10; q++){
						if (charStorage[q] != '\0'){
							minCPUtimeInput[q] = charStorage[q];
						}
					}
				} else if (maxCPUtimeInput[0] == '\0'){
					for (int q = 0; q < 10; q++){
						if (charStorage[q] != '\0'){
							maxCPUtimeInput[q] = charStorage[q];
						}
					}
				}
				// reset charStorage
				for(int x = 0; x < 122; x++){
					charStorage[x] = '\0';
				}

                // check if that was the last input
				bool finalInput = false;
				for (int i = secondSpace + 1; i < 126; i++){
					// check for non-null and non-space characters in the commandInput
					if (commandInput[i] != ' ' && commandInput[i] != '\0'){
						finalInput = true;
                        // for loop through to detect for any other spaces
                        // upon finding another space, check for a letter after it
                        // if so, not the last word
                        bool isItReallyLastWord = true;
                        for (int x = i + 1; x < 126; x++){
                            if (commandInput[x] == ' '){
                                if (commandInput[x+1] != ' ' && commandInput[x+1] != '\0'){
                                    isItReallyLastWord = false;
                                    break;
                                }
                            }
                        }
                        if (isItReallyLastWord == false){
                            finalInput = false;                     
                            break;
                        }
					}
				}
				
				// assign the final input
				if (finalInput == true){
					// fill the charStorage with the final input
					x = 0;
					for (int z = secondSpace + 1; z < 126; z++){
						if (commandInput[z] == ' ' || commandInput[z] == '\0'){
							break;
						}
						charStorage[x] = commandInput[z];
						x++;
					}
					// find what the final variable is meant to be
					if (benchmarkInput[0] == '\0'){
						for (int q = 0; q < 122; q++){
							if (charStorage[q] != '\0'){
								benchmarkInput[q] = charStorage[q];
							}
						}
					} else if (schedulingPolicyInput[0] == '\0'){
						for (int q = 0; q < 10; q++){
							if (charStorage[q] != '\0'){
								schedulingPolicyInput[q] = charStorage[q];
							}
						}
					} else if (numOfJobsInput[0] == '\0'){
						for (int q = 0; q < 10; q++){
							if (charStorage[q] != '\0'){
								numOfJobsInput[q] = charStorage[q];
							}
						}
					} else if (priorityLevelsInput[0] == '\0'){
						for (int q = 0; q < 10; q++){
							if (charStorage[q] != '\0'){
								priorityLevelsInput[q] = charStorage[q];
							}
						}
					} else if (minCPUtimeInput[0] == '\0'){
						for (int q = 0; q < 10; q++){
							if (charStorage[q] != '\0'){
								minCPUtimeInput[q] = charStorage[q];
							}
						}
					} else if (maxCPUtimeInput[0] == '\0'){
						for (int q = 0; q < 10; q++){
							if (charStorage[q] != '\0'){
								maxCPUtimeInput[q] = charStorage[q];
							}
						}
					}
					break;
				}
				firstSpace = secondSpace;
				secondSpace = 0; 
			}
		}

		if (benchmarkInput[0] == '\0'){
			x = 0;
			secondSpace = 3;
			for (int z = secondSpace + 1; z < 126; z++){
				if (commandInput[z] == ' ' || commandInput[z] == '\0'){
					break;
				}
				charStorage[x] = commandInput[z];
				x++;
			}
			for (int q = 0; q < 122; q++){
				if (charStorage[q] != '\0'){
					benchmarkInput[q] = charStorage[q];
				}
			}
		}
		
		int schedulingPolicy;
		int numOfJobs;
		int priorityLevels;
		int minCPUtime;
		int maxCPUtime;
		char benchmark[100] = {'\0'};
		
		sscanf(numOfJobsInput, "%d", &numOfJobs);
		sscanf(priorityLevelsInput, "%d", &priorityLevels);
		sscanf(minCPUtimeInput, "%d", &minCPUtime);
		sscanf(maxCPUtimeInput, "%d", &maxCPUtime);
		
		if (strcmp(schedulingPolicyInput, "fcfs") == 0){
			schedulingPolicy = 1;
		} else if (strcmp(schedulingPolicyInput, "sjf") == 0){
			schedulingPolicy = 2;
		} else if (strcmp(schedulingPolicyInput, "pri") == 0){
			schedulingPolicy = 3;
		}
		
		struct test testInput = {schedulingPolicy, numOfJobs, priorityLevels, minCPUtime, maxCPUtime, ""};
		strcpy(testInput.benchmark, benchmark);
		command = 7;
		struct inputStruct inputToReturn = {command, {0, 0.0, 0.0, 0.0, false, '\0'}, testInput};

        // reset commandInput
		for(int x = 0; x < 127; x++){
			commandInput[x] = '\0';
        }
		return inputToReturn;
		
	} else if (strcmp(userInputArray2, "quit") == 0){
		command = 8;
		struct inputStruct inputToReturn = {command, {0, 0.0, 0.0, 0.0, false, '\0'}, {0,0,0,0,0,'\0'}};
		return inputToReturn;
	}
	
    char userInputArray3[8] = {'\0'};
    strncpy(userInputArray3, commandInput, 8);  
    userInputArray3[8] = '\0';
	if (strcmp(userInputArray3, "priority") == 0){
		command = 4;
		struct inputStruct inputToReturn = {command, {0, 0.0, 0.0, 0.0, false, '\0'}, {0,0,0,0,0,'\0'}};
		return inputToReturn;
	}
	
	// At this point, if nothing has been returned, it means the user input a false command
	printf("Sorry, that command is not recognized, please double check your input and try again");
	struct inputStruct inputToReturn = {0, {0, 0.0, 0.0, 0.0, false, '\0'}, {0,0,0,0,0,'\0'}};
}

// sorting functions
void fcfs(){
	qsort (jobArray, 100, sizeof(struct job), compareFirstCome);
}

void sjf(){
	qsort (jobArray, 100, sizeof(struct job), compareShortestFirst);
}

void priority(){
	qsort (jobArray, 100, sizeof(struct job), comparePriority);
}

// functions to compare 2 different jobs for quick sorting
int compareFirstCome (const void *a, const void *b) {
    const struct job *jobA = a;    
    const struct job *jobB = b;

	if (jobA->arrivalTime < 1){
		return 1;
	} else if (jobB->arrivalTime < 1){
		return -1;
	}  
	
	if (jobA->arrivalTime < jobB->arrivalTime){
		return -1;
	} else if (jobA->arrivalTime > jobB->arrivalTime){
		return 1;
	} else {
		return 0;
	}
}

int compareShortestFirst (const void *a, const void *b) {
    const struct job *jobA = a;    
    const struct job *jobB = b;

	if (jobA->expectedRunTime < 1){
		return 1;
	} else if (jobB->expectedRunTime < 1){
		return -1;
	}  

	if (jobA->expectedRunTime < jobB->expectedRunTime){
		return -1;
	} else if (jobA->expectedRunTime > jobB->expectedRunTime){
		return 1;
	} else {
		return 0;
	}
}

int comparePriority (const void *a, const void *b) {
    const struct job *jobA = a;    
    const struct job *jobB = b;
    
    if (jobA->priority < 1){
		return 1;
	} else if (jobB->priority < 1){
		return -1;
	}

	if (jobA->priority < jobB->priority){
		return -1;
	} else if (jobA->priority > jobB->priority){
		return 1;
	} else {
		return 0;
	}
}
