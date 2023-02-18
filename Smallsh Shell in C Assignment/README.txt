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

__________________________________________________________________________________________
README TEXT FILE: 



The provided zip folder named "montanek_program3.zip" will have 4 files including: 

1. smallsh.c
3. README.txt (this file)

Upon zip extraction, the files will be available in the 
montanek/CS344/vsprojects/Assignment\3/ directory.
Directions for compiling smallsh.c are as follows: 

1. gcc --std=gnu99 -o smallsh smallsh.c
2. ./smallsh



The following execution uses the GNU99 compiler, and contains the
"#define _GNU_SOURCE" for ease of use if
the above command line for gnu99 is not used. 


