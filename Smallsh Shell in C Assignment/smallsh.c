/*
    Name:           Kristina Montanez
    Date:           Nov 1, 2021
    Class:          CS344
    Project:        Assignment 3
    Description:    Smallsh-shell in C. Provide a prompt for running commands, handling blank 
                    lines and comments, Provide expansion for the variable $$, Execute 3 commands 
                    including exit, cd, and status via code built into the shell, and other 
                    commands stemming from the exec family of functions. Program will also
                    support input, output, running commands in foreground and background, and 
                    implement custom handlers for 2 signals: SIGINT and SIGTSTP.

                    SCRIPT: p3testscript can be ran by the following commands:

                    1)  chmod +x ./p3testscript
                    2)  ./p3testscript > mytestresults 2>&1
______________________________________________________________________________________________

Include the usual headers from 
Assignments 1 and 2, and 
the new headers that were included
in this week's modules.  
<sys/types.h> is used for pid_t, and
<unistd.h> is used for fork.
include GNU option just in case.
*/

#define _GNU_SOURCE
#include <stdio.h>      // for printf and perror
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>     // for exit
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>     // for execvp, getpid, fork
#include <dirent.h>
#include <fcntl.h>
#include <sys/wait.h>   // for waitpid
#include <signal.h>     // for sigaction structs, and sigfillset- see Ed discussion 
                        // at https://edstem.org/us/courses/14269/discussion/800838
/*

______________________________________________________________________________________________
TABLE OF CONTENTS:

1) GLOBAL VARIABLES, STRUCTS, & FUNCTION DECLARATIONS:

    1.1) Definitions for the max requirements           
    1.2) Global Variables
    1.3) Structs
    1.4) Function Declarations

2) COMMAND LINE INPUT:

    2.1) readInput()            - Function to ask for and get user input.
    2.2) variableReplace()      - Function to replace all the "$$" in the string to the process ID.
    2.3) parseInput()           - Function to parse tokens from the input string.

3)  BUILT IN COMMANDS:

    3.1) cdCommand()            - Function to make a new directory.
    3.2) statusCommand()        - Makes a new file with movies that take place in the.
    3.3) exitCommand()          - Function to process a new directory, and files in the directory.

4)  CUSTOM HANDLERS:

    4.1) handle_SIGINT()        - Required Function to implement custom handler for SIGINT.
    4.2) handle_SIGTSTP()       - Required Function to implement custom handler for SIGTSTP.

5)  BACKGROUND PROCESS AND INPUT/OUTPUT REDIRECTION:

    5.1) backProcCheck()        - Function to show finished or terminated background processes.
    5.2) handle_SIGTSTP()       - Required Function to implement custom handler for SIGTSTP.

6)  EXECUTION FUNCTIONS:

    6.1) findCommand()           - Function to determine if command is a built in command or not.
    6.2) execCommands()          - Function to process commands not build in (cd, status, exit).

7)  MAIN FUNCTION:

    7.1) Global Variables       
    7.2) Current pids         
    7.3) Check User Inputs  
    7.4) Free Memory
______________________________________________________________________________________________






______________________________________________________________________________________________
1)  GLOBAL VARIABLES, STRUCTS, & FUNCTION DECLARATIONS
______________________________________________________________________________________________
*/

//1.1) Definitions for the max requirements
#define MAXLENGTH 2048
#define MAXARGS 512


//1.2) Global Variables
int current_pids[5];
int signum;
int termination_by_signal = 1;
int foreground_mode = 1;


// 1.3) Structs
// Initialize structs to be empty
// As shown in materials-"Exploration: Signal Handling API"
struct sigaction SIGINT_action = {0};
struct sigaction ignore_action = {0};
struct sigaction SIGTSTP_action;


// 1.4) Function Declarations:
//First Execution Functions
char * readInput(); 
char* variableReplace(char*, char *); 
char** parseInput(char*);
//Built In Commands
void cdCommand(char**); 
void statusCommand(int, int); 
void exitCommand(char**, char*);
//Custom Handlers
void handle_SIGINT(int); 
void handle_SIGTSTP(int);
// I/O Redirection and Background Checks 
void backProcCheck();
int inputOutput(char**, int); 
// Exec Function
int findCommand(char**, char*, int); 
int execCommands(char**);




/*
______________________________________________________________________________________________
2)  COMMAND LINE INPUT
______________________________________________________________________________________________
2.1) readInput()            - Function to ask for and get user input.
2.2) variableReplace()      - Function to replace all the "$$" in the string to the process ID.
2.3) parseInput()           - Function to parse tokens from the input string.
*/



/*2.1)_____________________________________________________________ 
Function to ask for and get user input. Calls the variableReplace 
function if "$$" is found. Returns the string pointer. 
*/
char* readInput()
    // Initiate the buffer, input, and 
    // maximum characters allowed in commmand line.
    {char* input = NULL;    
    char* buffer = NULL;
    size_t size = MAXLENGTH;
    //prompt the user for input.
    printf(": "); 
    fflush(stdout);
    //grab the input from user
    getline(&buffer, &size, stdin); 
    // If the buffer is not empty,
    // Look through string to replace variable.
    if (buffer != NULL)
        {buffer = variableReplace(buffer, input);}
    return buffer;}




/*2.2)_____________________________________________________________ 
Function to replace all the "$$" in the string to the process ID. 
Takes the original input, and the pointer to the buffer. Returns the
string with the replacement. 
*/
char* variableReplace(char * input, char* buffer)
    // Initialize our variable to expand and counter.
    {char process_id[] = "$$";
    int count;
    int get_pid = getpid();     
    char * $$_switch;
    // Check if there is no "$$"
    if (strstr(input, process_id) == NULL)
        {return input;}
    //While the string still has "$$" in it.
    while (strstr(input, process_id) != NULL)
        {int amount = (sizeof(char)) + 32 * sizeof(char);
        buffer = malloc(strlen(input) * amount);
        memset(buffer, '\0', strlen(buffer));
        $$_switch = strstr(input, process_id);      
        //Go through the characters until the variable is 
        //replaced. 
        count = 0;
        while(&input[count] != $$_switch)
            {strncat(buffer, &input[count], 1);
	        count++;}
        count = count + 2;
        //Print the buffer and the pid 
        sprintf(buffer, "%s%d", buffer, get_pid);
        //Add the rest of the string.
        while(count < strlen(input))
            {strncat(buffer, &input[count], 1);
	        count++;}
        //Don't forget the null terminator
        strcat(buffer, "\0");
        input = buffer;}
    return buffer;}






/*2.3)_____________________________________________________________ 
Function to parse tokens from the input string. Takes pointer to the 
individual tokens and returns pointers to each. 
*/
char** parseInput(char * token)
    // Initialize our delimiter, counter,
    {
    char delimiter[] = " \n\r\t";
    // and input size.
    int size = sizeof(char*);
    // Start our counter.
    int count = 0;
    // Set size of input.
    char** input = calloc(MAXARGS, size);
    char* parsed_token; 
    // Get first token
    parsed_token = strtok(token, delimiter); 
    while(parsed_token != NULL)
        // Pointer to current token.
        {input[count] = parsed_token; 
        // Update our count
        count++;
        // If there are too many arguments, let the 
        // user know and return.
        if (count >= MAXARGS)
            {printf("Command uses too many arguments.\n");
	        fflush(stdout);
	        return NULL;}
        // Get next token
        parsed_token = strtok(NULL, delimiter);}
    // reset our count for the next round.
    input[count] = NULL; 
    return input;}






/*
______________________________________________________________________________________________
3)  BUILT IN COMMANDS
______________________________________________________________________________________________
3.1) cdCommand()            - Function to change the directory to the desired path.
3.2) statusCommand()        - Function to print the exit value or terminated by signal.
3.3) exitCommand()          - Function to kill background processes.
*/



/*3.1)_____________________________________________________________ 
Function to change the directory to the desired path, or to the 
"HOME" directory if no path is specified. Takes the command list 
as an argument. Does not return a value.
*/
void cdCommand(char** command)
    {if (command[1] == NULL)
        // Change directory to home.
        {chdir(getenv("HOME"));}
    else
        // Use the command declaration 
        // to change to the directory.
        {chdir(command[1]);}}




/*3.2)_____________________________________________________________ 
Function to print the exit value (or if terminated by signal). Takes 
the signal number, and if terminated by signal. Does not return a value.
*/
void statusCommand(int exit_stat, int status)
    // If the process is not terminated by signal.
    {if (status == 0)
        // Print error message for user.
        {printf(" exit value %d\n", exit_stat);
        fflush(stdout);} 
    else 
        //If the process is terminated by signal.
        {printf(" terminated by signal %d\n", exit_stat);
        fflush(stdout);}}
      




/*3.3)_____________________________________________________________ 
Function to kill background processes, free up the memory before the 
user quits the shell. Takes the user's input, and pointers to parsed 
words from command line. Does not return a value.
*/
void exitCommand(char** command, char *input)
    //killing any background processes
    {for (int i = 0; i < 5; i++)
        // If processes are saved, kill that 
        // process, and zero it out.
        {if (current_pids[i] != 0)
            {kill(current_pids[i], SIGTERM);
	        current_pids[i] = 0;}}
    //freeing userinput  
    if (input != NULL)
        {free(input);}
    if (command != NULL)
        {free(command);}
    exit(0);}







/*
______________________________________________________________________________________________
4)  CUSTOM HANDLERS
______________________________________________________________________________________________
4.1) handle_SIGINT()        - Required Function to implement custom handler for SIGINT.
4.2) handle_SIGTSTP()       - Required Function to implement custom handler for SIGTSTP.
*/




/*4.1)_____________________________________________________________ 
Required Function to implement custom handler for SIGINT. Does not 
return a value, returns statusCommand function.
*/
void handle_SIGINT(int signo)
    // Reset our global variable for termination signal.
    {termination_by_signal = 1;
    // Our signal number is set for the 
    // status to send to statusCommand.
    signum = signo; 
    // Send to statusCommand to print
    // the status message to user.
    statusCommand(signo, 1);}



/*4.2)_____________________________________________________________ 
Required Function to implement custom handler for SIGTSTP. Prints a 
message for foreground-only mode, and switches the global variable 
from foreground to exiting foreground. Takes the signal number as a
parameter. Does not return a value.
*/
void handle_SIGTSTP(int signo)
    // If the signal number indicates foreground-only mode,
    // switch our signal number variable, and give message
    // to the User.
    {if (foreground_mode == 0)
        {foreground_mode = 1;
        char * message = "Entering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 49);}
    else
        // For exiting foreground-only mode, switch the signal number
        // variable, and let the user know that they are exiting 
        // foreground only mode.
        {foreground_mode = 0;
        char* message = "Exiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 29);}}






/*
______________________________________________________________________________________________
5)  BACKGROUND PROCESS AND INPUT/OUTPUT REDIRECTION
______________________________________________________________________________________________
5.1) backProcCheck()        - Function to show finished or terminated background processes.
5.2) inputOutput()          - Function to determine input and output redirection for a process.
*/





/*5.1)_____________________________________________________________ 
Function to show the ongoing background processes, that are either 
finshed or have been terminated. The exit status, or signal termination
will be shown if applicable. Does not take parameters or provide 
anything in return. 
*/
void backProcCheck()
    {int process;
    int exit_stat;
    for (int i = 0; i < 5; i++)
        {if(current_pids[i] != 0)
            //Make sure there are no background processess still going.
            {process = waitpid(current_pids[i], &exit_stat, WNOHANG);
	    if (process != 0)
            //The background process has finished.
	        {printf("background pid %d is done:", current_pids[i]);
	        fflush(stdout);
	        if (exit_stat == 1)
                //show exit stat
                {statusCommand(exit_stat, 0);}
            else if (exit_stat == 0)
                //show exit stat
                {statusCommand(exit_stat, 0);}
	        else
                //show termination stat
	            {statusCommand(exit_stat, 1);} 
	        // Take away the current pids
            current_pids[i] = 0;}}}}



/*5.2)_____________________________________________________________ 
Function to determine input and output redirection for a process, 
removing the < and > characters, and direction for file names. 
Returns a sucessful or unsucessful status.
*/
int inputOutput(char **input, int back_proc)
    // Keep our input and output symbols straight.
    {char in[] = "<";
    char out[] = ">";
    int i = 0;
    // Go through all the input tokens.
    while(input[i] != NULL)
        {if(strcmp(input[i], in) == 0) 
            // Attempt our input redirection.
            {int sourceFD = open(input[i+1], O_RDONLY);
            if (sourceFD == -1) 
                // If we cannot open our target file.
	            {if (back_proc == 1) 
                    //Redirect our background process from dev/null.
	                {int backFD = open("dev/null", O_RDONLY);
	                dup2(backFD, STDIN_FILENO);
	                close (backFD);
	                i++;
	                input[i-1] = NULL;}
	            else 
                    // Print error message for user.
	                {printf("cannot open %s for input\n", input[i+1]);
	                return 0;}}
	        else
                // If the file open is sucessful.
                {dup2(sourceFD, STDIN_FILENO);
                close(sourceFD);
                input++;
	            input[i-1] = NULL;
	            input++;
	            input[i-1] = NULL;}}
        else if (strcmp(input[i], out) == 0) 
            // redirect output, provide set of permissions.
	        {int targetFD = open(input[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (targetFD == -1) 
                // If the targetFD failes to create.
	            {if (back_proc == 0) 
                    // Show error for user, and return false.
	                {printf("cannot open or create %s for output\n", input[i+1]);
	                return 0;}
                else
                    // Redirect output to /dev/null
                    {int out_redirect = open("dev/null", 0644);
                    dup2(out_redirect, STDOUT_FILENO);
                    close(targetFD);
                    input++;
                    input[i-1] = NULL;}}
            else 
                //sucessful file open in foregrond or background
                {dup2(targetFD, STDOUT_FILENO);
                close(targetFD); 
                input++;
                input[i-1] = NULL;
                input++;
                input[i-1] = NULL;}}
        else 
	        {i++;}}
    return 1;}








/*
______________________________________________________________________________________________
6)  EXECUTION FUNCTIONS
______________________________________________________________________________________________
6.1) findCommand()           - Function to determine if command is a built in command or not.
6.2) execCommands()          - Function to process commands not build in (cd, status, exit).
*/


/*6.1)_____________________________________________________________ 
Function to determine if command line has any of the three built in 
commands, including "cd", "status", and "exit". If the command does 
not contain any of these, it moves to non-built in commands. If the 
command is cd or status, the last status signal is returned. 
*/
int findCommand(char**command, char *input, int last_stat_sig)
    //If the user typed to change directory;
    {if (strcmp(*command, "cd") == 0)
        {cdCommand(command);
	    return last_stat_sig;}
    // If the user typed status.
    else if(strcmp(*command, "status") == 0)
        {statusCommand(last_stat_sig, termination_by_signal);
	    return last_stat_sig;}
    //If the user typed to exit.
    else if (strcmp(*command, "exit") == 0)
        {exitCommand(command, input);}
    else
        //If the user did not type a build in command.
        {return execCommands(command);}}





/*6.2)_____________________________________________________________ 
Function non-built in commands. Will check if new process fails, and 
if not, starts child process, checks for background processes, checks
for input/output redirection if needed, checks SIGINT/SIGTSTP, and 
returns exit status or termination signal. 
*/
int execCommands(char** command)
    // Set our global variable
    {termination_by_signal = 0;  
    int childStatus = -3;
    int input_output;
    // Fork, exec, and File Descriptor Inheritance, 
    // As shown in materials in Exploration: Process and I/O
    pid_t spawnPid = -3;
    // Fork a new process
    spawnPid = fork();
    switch (spawnPid)
        {case -1:
            // If the fork failed, exit.
            {perror("fork() failed\n");
            exit(1);
            return 1;}
        case 0:
            // Start the child process.
            // ignore the SIGSTP signal.
            {sigaction(SIGTSTP, &ignore_action, NULL); 
            //ignore sigtstp signal
            int back_proc = 0;  
            int i=0;
            while (command[i]!= NULL)
                {if(strcmp(command[i], "&") == 0 && command[i+1] == NULL)
                    // If the & character is last character.
                    {command[i] = NULL; 
                    // Remove the & character from the 
                    // arguments list.
                    if(foreground_mode == 1)
                        // If in foreground mode, make sure 
                        // to ignore the & character.
                        {back_proc = 0;
                        break;}
                    back_proc = 1;}
                    i++;}
		    if (back_proc == 1)
                // If it's background process it will
                // ignore the SIGINT. 
                {sigaction(SIGINT, &ignore_action, NULL);}
		    input_output = inputOutput(command, back_proc); 
            //perform i/o redirection if any
		    if (input_output == 0)
                // If the input/output was unsucessful.
                {exit(1);}
		execvp(command[0], command); 
        // Give the error message.
		perror(command[0]); 
        //there was an error performing command
		exit(1);}
      default:
            // Take care of the parent process
	         {sigaction(SIGTSTP, &SIGTSTP_action, NULL); 
             // Take care of SIGINT (ignore it)
		 sigaction(SIGINT, &ignore_action, NULL); 
		int back_proc = 0;  
            int i = 0;
            while (command[i] != NULL)
                {if(strcmp(command[i], "&") == 0 && command[i+1] == NULL)
                    // Check if the & character is last character.
                    {command[i] = NULL; 
                    // Remove the & character from the 
                    // arguments list.
                    if(foreground_mode == 1)
                        // If in foreground mode, make sure 
                        // to ignore the & character.
                        {back_proc = 0;
                        break;}
                    back_proc = 1;}
                    i++;}
		    if (back_proc == 1)
                // Print the background pid for user.
                {printf("background pid is %d\n", spawnPid); 
		        fflush(stdout);
                for (int i = 0; i < 5; i++)
                    // If the current_pids are empty,
                    // add the id to the process array.
                    {if (current_pids[i] == 0)
                        {current_pids[i] = spawnPid;
                        break;}}
		        return 0;}
		    else
            // While process is running in the 
            // background, we wait for the current process.
            {pid_t pid = waitpid(spawnPid, &childStatus, 0); 
		    if (childStatus == 0)
                // If our program exists without error.
                {return 0;}
		    if (termination_by_signal == 1) 
                // If our program is terminated.
		       {return signum;}
		    return 1;}
		 break;
		 exit(1);}}}








   

/*
______________________________________________________________________________________________
7)  MAIN FUNCTION
______________________________________________________________________________________________
7.1) Global Variables       
7.2) Current pids         
7.3) Check User Inputs  
7.4) Free Memory    
*/


int main()
/*7.1)_____________________________________________________________ 
Make sure our global variables, structs, and handlers are set up.
*/ 
    // Global Variables set.
    {termination_by_signal = 0;
    foreground_mode = 0;

    // Set our local variables for user input,
    // parsed input, and running shell
    char * input;
    char ** command;
    int still_going = -3;

    // As it shows in the materials: 
    // Exploration: Signal Handling API
    // Our ignore_action struct.
    ignore_action.sa_handler = SIG_IGN;

    // As it shows in the materials: 
    // Exploration: Signal Handling API
    // Our sigtstp_action struct.
    SIGTSTP_action.sa_handler = handle_SIGTSTP;
    sigfillset(&SIGTSTP_action.sa_mask);
    // No flags set
    SIGTSTP_action.sa_flags = 0;

    // As it shows in the materials: 
    // Exploration: Signal Handling API
    // Our sigint_action struct. 
    SIGINT_action.sa_handler = handle_SIGINT;
    sigfillset(&SIGINT_action.sa_mask);
    // No flags set
    SIGINT_action.sa_flags = 0;
    
    // Register the ignore_action as the handler for SIGINT
	// So so that it will be ignored.
    sigaction(SIGINT, &ignore_action, NULL);
    // Set up the SIGTSTP
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

/*7.2)_____________________________________________________________ 
Current pids in check, and variables for running our shell.
*/
    for (int i = 0; i < 5; i++)
        {current_pids[i] = 0;}
   
/*7.3)_____________________________________________________________ 
Keep checking user's inputs.
*/

    while (true)
        // First, make sure we don't have any processes
        // completed when command comes in.
        {backProcCheck(); 
        // Request the input from the user.
        input = readInput(); 
        // Parse out the command from the user.
        command = parseInput(input); 
        // Check for comments or empty lines.
        if (command[0] != NULL && strncmp(command[0], "#", 1) != 0)
            // Determine if command is built-in command or not.
            {still_going = findCommand(command, input, still_going);}

/*7.4)_____________________________________________________________
At last, we can free up our memory.  
*/
        if (input != NULL)
            {free(input);}}
        if (command != NULL)
            {free(command);}}