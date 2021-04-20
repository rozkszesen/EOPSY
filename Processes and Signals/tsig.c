#include <stdio.h>
#include <sys/types.h> 
#include <sys/wait.h> //wait()
#include <unistd.h> //fork() 
#include <signal.h> //SIGTERM etc

#define NUM_CHILD 5 //number of children to be born
#define WITH_SIGNALS //preprocessor symbol for a version with signal handling

void killChildren(int childrenCount, int childrenPIDs[]);
void waitForChildrenToDie();

#ifdef WITH_SIGNALS
//signals handling
int keyboardInterruptOccured = 0;//global variable for keyboard interrupt
void ignoreSignals();
void setKeyboardInterruptHandler();
void keyboardInterruptHandler(int signum);
void setTerminationHandler();
void terminationHandler(int signum);
void restoreAllDefaultHandlers();
#endif

int main()
{
	pid_t childrenPIDs[NUM_CHILD]; //table with PIDs of born children
	int childrenCount;
	int parentPID = (int)getpid();

	for(childrenCount = 0; childrenCount < NUM_CHILD; childrenCount++)
	{
		pid_t pid = fork(); //>0 if a parent, 0 if a child, <0 if failure
		int returnValue = (int)pid;
		
		//parent process
		if(returnValue > 0)
		{
			//save the child's pid (needed for killChildren() function)
			childrenPIDs[childrenCount] = pid;
			
			#ifdef WITH_SIGNALS
			//force ignoring all the signals
			ignoreSignals();
			
			//restore the default handler for SIGCHLD signal
			signal(SIGCHLD, SIG_DFL);
			
			//set keyboard interrupt handler
			setKeyboardInterruptHandler();
			#endif //WITH_SIGNALS

			sleep(1);
		}
		//child process
		else if(returnValue == 0)
		{
			#ifdef WITH_SIGNALS
			//ignore handling of keyboard interrupt signal
			signal(SIGINT, SIG_IGN);
			
			//set handler for SIGTERM signal
			setTerminationHandler();
			#endif //WITH_SIGNALS

			printf("child[%d]: Hello, parent[%d]\n", (int)getpid(), (int)getppid());
			sleep(10);
			printf("child[%d]:I'm dying, bye\n", (int)getpid());
			return 0;
		}
		//failure case
		else if(returnValue < 0)
		{
			//SIGTERM signal -> use kill()
			printf("parent[%d]: sending SIGTERM signal - killing all my children :(\n", parentPID);
			killChildren(childrenCount, childrenPIDs); //kill all previously born children
			return 1;
		}

		#ifdef WITH_SIGNALS
		if(keyboardInterruptOccured != 0)
		{
			printf("parent[%d]: sending SIGTERM signal - killing all my children :(\n", parentPID);
			killChildren(childrenCount, childrenPIDs);
			break;
		}
		#endif //WITH_SIGNALS
	}


	if(childrenCount == NUM_CHILD)
		printf("All children have been born!\n");

	//wait for all children to terminate, count all children
	waitForChildrenToDie();

	#ifdef WITH_SIGNALS
	//restore old service handlers for all signals
	restoreAllDefaultHandlers();
	#endif //WITH_SIGNALS

	return 0;
}

//---------------------------------------------------------------------------

void killChildren(int childrenCount, int childrenPIDs[])
{
	while(childrenCount >= 0)
	{
		printf("child[%d]: received SIGTERM signal - I'm dying :(\n", childrenPIDs[childrenCount]);
		kill(childrenPIDs[childrenCount], SIGTERM);
		childrenCount--;
	}
}

void waitForChildrenToDie()
{
	int count = 0;
	int parentPID = (int)getpid(); //get parent pid
	pid_t pid;

	printf("parent[%d]: Waiting for my children to die... \n", parentPID);
	//wait for death of all children including zombies, read their PIDs
	while((int)wait(NULL) != -1)
	{
		printf("parent[%d]: Waiting for my child to die...\n", parentPID);
		count++;
	}
	//wait(NULL) returns -1 in case of failure - so when there are no more child processes
	printf("parent[%d]: No more child processes - all children died :(\n", parentPID);
	printf("parent[%d]: Number of received child processes exit codes = %d\n", parentPID, count);

}

#ifdef WITH_SIGNALS
void ignoreSignals()
{
	//iterate through all signals and ignore each one of them (NSIG - number of all defined signals)
	for(int i = 1; i < NSIG; i++)
	{
		//SIG_IGN - disposition to ignore the signal
		signal(i, SIG_IGN);
	}
}

void setKeyboardInterruptHandler()
{
	//set handler for SIGINT
	signal(SIGINT, keyboardInterruptHandler);
}

void keyboardInterruptHandler(int signum)
{
	//notify about the keyboard interrupt
	printf("Received keyboard interrupt\n");
	//update the global variable
	keyboardInterruptOccured = 1;
}

void setTerminationHandler()
{
	//set handler for SIGTERM
	signal(SIGTERM, terminationHandler);
}

void terminationHandler(int signum)
{
	printf("child[%d]: process terminated\n", (int)getpid());
}

void restoreAllDefaultHandlers()
{
	for(int i = 1; i < NSIG; i++)
	{
		//restore the default handling for all signals
		signal(i, SIG_DFL);
	}

	printf("Restored default handlers for all signals\n");
}
#endif //WITH_SIGNALS
