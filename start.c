
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*

The task is to construct a small shell implementing the following:
	- a built-in command "exit" which terminates all remaining processes started from the
	 shell in an orderly manner before exiting the shell itself
	- a built-in command "cd" which should behave exactly as a normal built-in command
	 "cd" with the exception that you do not need explicitly to update environment
	 variables.
	- a built-in command "checkEnv" which should execute "printenv | sort | pager" if no
	 arguments are given to the command. If arguments are passed to the command then 
	 "printenv | grep <arguments> | sort | pager" should be executed. The pager executed 
	 should be selected primarily based on the value of the users "PAGER" environment 
	 variable. If no such variable is set then one should first try to execute "less" and 
	 if that fails "more". Any errors in the execution of the pipeline should be handled 
	 in a nice manner (i.e. similar to how your normal shell handles errors in the 
	 execution of a manually set up pipeline).
	- It should handle the execution of processes in foreground and background. For 
	 foreground processes the total execution time for the process should be printed when 
	 it terminates. For background process information on that they have terminated should 
	 be printed no later than when the shell prompts next time after the background 
	 process/es have terminated.
	- Detection of terminated background process should be implemented by two mechanisms
	 where one selects which should be compiled at compilation time. The mechanisms are 
	 ordinary polling and detection by signals sent from the child processes. The user 
	 should be able to select which mechanism to use by defining a macro SIGDET=1 at 
	 compilation time to have termination detected by signals. If SIGDET is undefined or 
	 equals zero then termination should be detected by polling.


The lab should be written in ANSI standard C. (the reason for this is that one can always
 assume that even old systems adhere to this standard and the differences to more recent
 standards as C99 is not that huge)
The return value from all system calls have to be checked for errors.
All termination of processes started from the shell must be handled correctly as not to 
 leave a possibility for creating zombie-processes
The code should compile without warnings on shell.ict.kth.se when compiled with the 
 command "gcc -pedantic -Wall -ANSI -O4"
The shell should behave controlled, i.e. as your normal shell (bash, tcsh) for all
 reasonable input to the shell and to processes started from the shell (for the latter
  check what happens if you terminate a child process with Ctrl-C"
You are not allowed to keep lists of your own for background processes or communicate with
 a signal handler by global variables. This simple shell does not require that - this
 restriction forces you to better understand what the OS is doing (i.e.do not do anything
 unnecessary)
You may use the POSIX library functions sighold() and sigrelse() if available.
You need not be able to handle longer command-line input than 80 characters.
You are not allowed to use excessive memory or have "memory leaks" (i.e. the amount of
 memory allocated through malloc(3C) may not grow over time - however you probably do not
 need to allocate any memory in this way)
 
 */

/* A process is a single process.  */
typedef struct process
{
  struct process *next;       /* next process in pipeline */
  char **argv;                /* for exec */
  pid_t pid;                  /* process ID */
  char completed;             /* true if process has completed */
  char stopped;               /* true if process has stopped */
  int status;                 /* reported status value */
} process;

/* A job is a pipeline of processes.  */
typedef struct job
{
  struct job *next;           /* next active job */
  char *command;              /* command line, used for messages */
  process *first_process;     /* list of processes in this job */
  pid_t pgid;                 /* process group ID */
  char notified;              /* true if user told about stopped job */
  struct termios;// tmodes;      /* saved terminal modes */
  int stdin, stdout, stderr;  /* standard i/o channels */
} job;

/* The active jobs are linked into a list.  This is its head.   */
job *first_job = NULL;

/* Find the active job with the indicated pgid.  */
job * find_job (pid_t pgid)
{
  job *j;

  for (j = first_job; j; j = j->next)
    if (j->pgid == pgid)
      return j;
  return NULL;
}

/* Return true if all processes in the job have stopped or completed.  */
int job_is_stopped (job *j)
{
  process *p;

  for (p = j->first_process; p; p = p->next)
    if (!p->completed && !p->stopped)
      return 0;
  return 1;
}

/* Return true if all processes in the job have completed.  */
int job_is_completed (job *j)
{
  process *p;
  for (p = j->first_process; p; p = p->next)
    if (!p->completed)
      return 0;
  return 1;
}


int main(int argc, char **argv, char **envp)
{
	char str[80];
	while (1) {
	
		printf("$ ");
		gets( str );
		
		if(str[0] == 'c')
		{
			if(str[1] == 'd')
			{
				if(str[2] == ' ')
				{
					char dir[77]; //Only 77 possible (80 - cd+blankspace)
					int iterator = 3;
					for( int i = 0; i < 77; i++ )
					{
						while(str[iterator] == ' ') //Skip blankspaces. 
							iterator++;
						dir[i] = str[iterator];
						iterator++;
					}
					
					printf("-bash: cd: %s: No such directory\n", dir);
					continue;			
				}
				
			}
		} 
		else if(str[0] == 'e')
			if(str[1] == 'x')
				if(str[2] == 'i')
					if(str[3] == 't')
						break;		
						//TODO: Also stop all processes.. 
		
		//TODO: other commands and what to do with them... :/
		printf("-bash: %s: command not found\n", str);
	}
	
	
	return 0;
}


