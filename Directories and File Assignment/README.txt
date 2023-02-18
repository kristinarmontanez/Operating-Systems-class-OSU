Name:           Kristina Montanez
Date:           October 18, 2021
Class:          CS344
Project:        Assignment 2
Description:    Program that can read any smallest, largest, or specified CSV file in the 
                folder, and allow the user to process the file into a new directory, which 
                is named "your_onid.movies.random". The "random" wil be a series of numbers
                between 0-00000. User will have read/write/execute permissions. The new 
                directory will contain a text file for each year that movie was released,
                each having the movie titles on separate lines.  

__________________________________________________________________________________________
README TEXT FILE: 



The provided zip folder named "montanek_program2.zip" will have 4 files including: 

1. main.c
2. movies_by_year (executable file)
3. README.txt (this file)
4. movies_sample_1.CSV

Upon zip extraction, the files will be available in the 
montanek/CS344/vsprojects/Assignment\2/ directory.
Directions for compiling main.c are as follows: 

1. gcc --std=gnu99 -o movies_by_year main.c
2. ./movies_by_year 



The following execution uses the GNU99 compiler, and contains the
"#define _GNU_SOURCE" for ease of use if
the above command line for gnu99 is not used. 


