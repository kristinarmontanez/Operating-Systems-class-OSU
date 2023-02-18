Name:           Kristina Montanez
Date:           Nov 15, 2021
Class:          CS344
Project:        Assignment 4
Description:    Program that will create 4 threads to take input and read in lines of 1000
                characters max, 50 lines max from the standard input, call a second thread
                for replacing line separators with spaces, a third thread to replace "++"
                with "^", and a fourth thread to write the output to lines of 80 characters.
                The 4 threads communicate with each other using the "Producer-Consumer"
                approach. Input will only consist of ASCII characters (space-32 to tilde-126)
                and the program will stop when only characters in the line are "STOP" followed
                by the line separator. 

                Example code provided for assignment: 
                "6_5_prod_cons_pipeline.c"

__________________________________________________________________________________________
README TEXT FILE: 



The provided zip folder named "montanek_program4.zip" will have 4 files including: 

1. line_processor.c
3. README.txt (this file)

Upon zip extraction, the files will be available in the 
montanek/CS344/vsprojects/Assignment\4/ directory.
Directions for compiling line_processor.c are as follows: 

1. gcc -std=gnu99 -pthread -o line_processor line_processor.c
2. ./line_processor



The following execution uses the GNU99 compiler, and contains the
"#define _GNU_SOURCE" for ease of use if
the above command line for gnu99 is not used. 


