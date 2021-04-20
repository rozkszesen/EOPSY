#include <stdio.h>
#include <sys/types.h> 
#include <sys/wait.h> //wait()
#include <unistd.h> //fork() 
#include <signal.h> //SIGTERM etc

#define NUM_CHILD 5 //number of children to be born
int keyboardInterruptOccured = 0;//global variable for keyboard interrupt

void killChildren(int childrenCount, int childrenPIDs[]);
void waitForChildrenToDie();

//signals handling
void ignoreSignals();
void setKeyboardInterruptHandler();
void keyboardInterruptHandler(int signum);
void setTerminationHandler();
void terminationHandler(int signum);
void restoreAllDefaultHandlers();

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
			
			//force ignoring all the signals
			ignoreSignals();
			
			//restore the default handler for SIGCHLD signal
			signal(SIGCHLD, SIG_DFL);
			
			//set keyboard interrupt handler
			setKeyboardInterruptHandler();

			sleep(1);
		}
		//child process
		else if(returnValue == 0)
		{
			//ignore handling of keyboard interrupt signal
			signal(SIGINT, SIG_IGN);
			
			//set handler for SIGTERM signal
			setTerminationHandler();

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

		if(keyboardInterruptOccured != 0)
		{
			printf("parent[%d]: sending SIGTERM signal - killing all my children :(\n", parentPID);
			killChildren(childrenCount, childrenPIDs);
			break;
		}
	}

	//exited the for loop - all children are born
	printf("All children have been born!\n");

	//wait for all children to terminate, count all children
	waitForChildrenToDie();

	//restore old service handlers for all signals
	restoreAllDefaultHandlers();

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
	int pid;

	printf("parent[%d]: Waiting for my children to die... \n", parentPID);
	//wait for death of all children including zombies, read their PIDs
	while((int)wait(&pid)!= -1)
	{
		printf("parent[%d]: Waiting for child[%d] to die...\n", parentPID, pid);
		count++;
	}
	//wait(NULL) returns -1 in case of failure - so when there are no more child processes
	printf("parent[%d]: No more child processes - all children died :(\n", parentPID);
	printf("parent[%d]: Number of received child processes exit codes = %d\n", parentPID, count);

}

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
