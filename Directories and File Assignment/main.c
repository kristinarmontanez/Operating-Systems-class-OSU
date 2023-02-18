/*
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
______________________________________________________________________________________________
*/

//Include the usual headers, and 
// the new headers that were included
// in this week's modules. 
//include GNU option just in case.
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
/*

______________________________________________________________________________________________
TABLE OF CONTENTS:

1) STRUCTS FOR MOVIES:

    1.1) Struct movie           - struct to organize movie information.
    1.2) Struct createMovie     - to parse information from each line.
    1.3) Struct processFile     - to process the file for the linked list.

2) FUNCTIONS:

    2.1) createDir()            - Function to make a new directory.
    2.2) createFiles()          - Makes a new file with movies that take place in the.
    2.3) processNew()           - Function to process a new directory, and files in the directory.
    2.4) small_large_files()    - Function to print out the highest rated movie in each year.
    2.5) specificFile()         - Function to find a specific file in the current directory.
    2.6) askQuestions()         - Function for initial and follow up questions.

3) MAIN FUNCTION:

    3.1) Invalid Handling   - Invalid file processing.
    3.2) Run Program        - Run initial function "askQuestions()"
    3.3) Exit Program       - Exit program upon User typing initial option "2".











______________________________________________________________________________________________
______________________________________________________________________________________________
1)  STRUCTS FOR MOVIES
______________________________________________________________________________________________
1.1) Struct movie           - movie information.
1.2) Struct createMovie     - to parse information from each line.
1.3) Struct processFile     - to process the file for the linked list.
*/




// 1.1)_____________________________________________________________
// Struct for movie information. The information will parsed, and 
// will have title, year, languages, and ratingValue for each movie.
// This code is used from the demo "students" from assignment 1.
struct movie 
    {
    char *title;
    int year;
    char *languages;
    float ratingValue;
    struct movie *next;
    };




// 1.2)_____________________________________________________________ 
// Parse the current line which is comma delimited and create a
// movie struct with the data in this line. This code is taken from
// the "students" demo provided in class, as assignment instructions
// allow for, as in assignment 1.
struct movie *createMovie(char *currLine)
    {
    struct movie *currMovie = malloc(sizeof(struct movie));

    // For use with strtok_r
    char *saveptr;

    // The first token is the title
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // The second token is the year
    int year_token = atoi(strtok_r(NULL, ",", &saveptr));
    currMovie->year = year_token;

    // The third token is the languages
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->languages = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->languages, token);

    // The fourth token is the ratingValue
    float rating_token = strtod(saveptr, &saveptr);
    currMovie->ratingValue = rating_token;

    // Set the next node to NULL in the newly created movie entry
    currMovie->next = NULL;

    // Finally, return the current movie.
    return currMovie;
    }




// 1.3)_____________________________________________________________
// Return a linked list of movies by parsing data from
// each line of the specified file. This code is taken from 
// the "students" demo provided in class, as assignment instructions
// allow for, as in assignment 1. 
struct movie *processFile(char *filePath)
    {
    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    //char *token;

    // The head of the linked list
    struct movie *head = NULL;
    // The tail of the linked list
    struct movie *tail = NULL;

    // Read the file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
        {
        // Get a new movie node corresponding to the current line
        struct movie *newNode = createMovie(currLine);
        // Is this the first node in the linked list
        if (head == NULL)
            {
            // This is the first node in the linked link
            // Set the head and the tail to this node
            head = newNode;
            tail = newNode;
            }
        else
            {
            // This is not the first node.
            // Add this node to the list and advance the tail
            tail->next = newNode;
            tail = newNode;
            }  
        }

    //Make sure we free up memory leak. 
    free(currLine);
    //Close stream.
    fclose(movieFile);
    return head;
    }








/*
______________________________________________________________________________________________
2)  FUNCTIONS
______________________________________________________________________________________________
2.1) createDir()            - Function to make a new directory.
2.2) createFiles()          - Makes a new file with movies that take place in the.
2.3) processNew()           - Function to process a new directory, and files in the directory.
2.4) small_large_files()    - Function to print out the highest rated movie in each year.
2.5) specificFile()         - Function to find a specific file in the current directory.
2.6) askQuestions()         - Function for initial and follow up questions.
*/





// 2.1)_____________________________________________________________
// Function to make a new directory with either the largest directory,
// smallest directory, or a specfic directory name that the user
// provides. 
char createDir(char *directory_name) 
    {
    // use the random() as mentioned in the assignment. 
    // https://stackoverflow.com/questions/19976634/randomint-and-randomize-in-c
    sprintf(directory_name, "montanek.movies.%ld", (random()%100000));
    // The permission on directories need to be "rwxr-x---".
    // https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html
    mkdir(directory_name, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP);
    return *directory_name;
    }








// 2.2)_____________________________________________________________
// Makes a new file with the list of movies that take place in the 
// year the txt file has as a name. The permissions that are required 
// for files are "rw-r-----". 
void createFiles(struct movie *new_dir, char *new_file_path) 
    {
    // Start with our file_descriptor just like in Exploration:files.
    int file_descriptor;
    // We get a 0 for the first line in the file, as there is no year
    // to use, which causes this "0.txt" to be created. Therefore, 
    // create a var to compare. 
    char first_line[6] = "0.txt";
    // Start our loop.
    while (new_dir != NULL) 
        {
        // Make our next txt file formatted output to a string. 
        sprintf(new_file_path, "%d.txt", new_dir->year);
        // We check if we have the first line, which causes a 0.
        if (strcmp(new_file_path, first_line) != 0) 
            {
            //  As shown in material from module Exploration: Files. 
            // Here, we also use the "S_IRGRP" so that we have all the 
            // "rw-r-----" permissions. 
            // https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html
            file_descriptor = open(new_file_path, O_RDWR | O_CREAT | O_APPEND,
                                                S_IRGRP | S_IRUSR | S_IWUSR);
            // write the movie into the file.
            write(file_descriptor, new_dir->title, strlen(new_dir->title));
            write(file_descriptor, "\n", strlen("\n"));
            close(file_descriptor);
            }
        // Move to the next file.
        new_dir = new_dir->next;}}







// 2.3)_____________________________________________________________           
// Function to process a new directory, and files in the directory.
// function uses both createDir(), and createFiles(). 
void processNew(char *newName)
        {
        printf("Now processing the chosen file named %s\n", newName);
        
        // Can't be more than 21 if largest number is 5 digits.
        char dir_name[21];
        // Creates a new directory
        createDir
    (dir_name);
        // Tell the user that you created the directory.
        printf("Created directory with name %s\n", dir_name);
        struct movie *list = processFile(newName);
        // Need 8 spaces, including null 0.
        char file_name[9];
        // from Exploration: Directories module, create a struct
        // from dirent header. Open the current directory. 
        DIR* currDir = opendir(".");
        struct dirent *aDir;
        while ((aDir = readdir(currDir)) != NULL) 
            {
            // loop through the new struct. To change our working 
            // directory, user chdir(). 
            // https://man7.org/linux/man-pages/man2/chdir.2.html
            if (strncmp(dir_name, aDir->d_name, strlen(dir_name)) == 0) 
                {
                chdir(dir_name);
                break;}}
        // Next, create our new files, with the movies lists.
        createFiles(list, file_name);
        chdir("..");
        closedir(currDir);}





        

// 2.4)_____________________________________________________________           
// Function to find the smallest or largest file in the current 
// directory. Much of the code is used from the modules, including
// Exploration: Directories.  
void small_large_files(int fileChoice) 
    {
    // from Exploration: Directories module, create a struct
    // from dirent header. Open the current directory. 
    DIR* currDir = opendir(".");
    struct dirent *aDir;
    // if our file exists, keep track of it.
    int file_exists = 0;
    // We can now keep track of the largest and smallest.
    struct stat maxStatDir;
    struct stat minStatDir;
    //create our pointer. 
    char *ptr;
    // start our file sizes for both smallest and largest. 
    off_t maxSize = 0;
    off_t minSize = 0;
    // create our comparison length.
    int compare = strlen("movies_");
    // create a count so we don't get repeating movie names in each
    // text file. 
    int movie_count = 0;
    // create placeholders for our smallest and our largest file names.
    char newMaxName[500];
    char newMinName[500];
    // from Exploration: Directories module, go through all the 
    // directories. 
    while ((aDir = readdir(currDir)) != NULL) 
        {
        // create a pointer for the last time we see "." in the name.
        // the ASCII character for . is 64. 
        ptr = strrchr(aDir->d_name, 46);
        // If file does not have an extension, skip check to
        // avoid segmentation fault with null ptr.
        if (ptr != NULL) 
            {
            // If the file has the .csv extension, and the prefix of the 
            // name starts with "movies_" as required by the assignment, 
            // we can then include it as a candidate.
            if (strcmp(".csv", ptr) == 0 && strncmp("movies_", aDir->d_name, compare) == 0) 
                {
                // We can then check the stats of each. 
                stat(aDir->d_name, &maxStatDir);
                stat(aDir->d_name, &minStatDir);
                // If the next file size is bigger, make it the current
                // biggest file. Don't forget to keep a movie list count, 
                // or else you get duplicate movies for each txt file. 
                if (movie_count == 0 || maxStatDir.st_size > maxSize) 
                    {
                    maxSize = maxStatDir.st_size;
                    memset(newMaxName, '\0', sizeof(newMaxName));
                    strcpy(newMaxName, aDir->d_name);
                    }
                // If the file is smaller, we can add it to the smallest 
                // ranking file.
                if (movie_count == 0 || minStatDir.st_size < minSize) 
                    {
                    minSize = minStatDir.st_size;
                    memset(newMinName, '\0', sizeof(newMinName));
                    strcpy(newMinName, aDir->d_name);
                    }
                // keep track of our list count. 
                movie_count++;
                // if we come across a file, we can check if off as found.
                file_exists = 1;}}}
        closedir(currDir);
        // free the struct. 
        free(aDir); 
        if (file_exists == 1)
            {
            // If user chose the largest file, process the max placeholder.
            if (fileChoice == 1)
                {
                processNew(newMaxName);
                }
            // If the user chose the smallest file, process the min placeholder.
            if (fileChoice == 2)
                {
                processNew(newMinName);}}}
        









// 2.5)_____________________________________________________________           
// Function to find a specific file in the current directory. The 
// function will request a specific file name, and if the file exists,
// the function will provide the name to the secondary function 
// "processNew()"", which will take care of the new directory with 
// the given name.
void specificFile ()
    {
    int file_exists = 0;
    // from Exploration: Directories module, create a struct
    // from dirent header. Open the current directory. 
    DIR* currDir = opendir(".");
    struct dirent *aDir;
    char *filePath = calloc(500, sizeof(char));
    // Get the file name from the user.
    printf("Enter the complete file name: ");
    scanf("%s", filePath);
    // from Exploration: Directories module, go through all the 
    // directories. 
    while ((aDir = readdir(currDir)) != NULL) 
        {
        // if the file is found, we then send the name to processNew().
        if (strcmp(aDir->d_name, filePath) == 0)
            {
            file_exists = 1;
            processNew(filePath);
            }
        }
        // if the file name does not exist, we warn the user.
        if (file_exists == 0)
            {
             printf("The file %s was not found. Try again\n", filePath);   
            }
    // close our directory.
    closedir(currDir);
    // free the struct.
    free(aDir);}







// 2.6)_____________________________________________________________
// Function to start the initial questions for the user, asking if
// they would like to process a csv file. The function uses a switch 
// for each option, and will process the smallest, the largest, or  
// specific file. If the user chooses #2, the program will exit back 
// back to main().
void askQuestions()
    {
    // Variables to hold the initial and subsequent choices from user.
    int first_choice; 
    int second_choice;
    // create a placeholder for when the user chooses to exit the program.
    int program_done = 0;
    // If the user choses to process a file, then allow them a second 
    // choice. 
    while (program_done == 0)
        {
        // Print out the instructions for the user's initial choice. 
        printf("1. Select file to process\n\
2. Exit the program\n\
\n\
Enter a choice 1 or 2: ");

    // Grab the user's initial input. 
    scanf("%d", &first_choice);
        // If the user exits: 
        if (first_choice == 2)
            {
            // exit the entire function to main(). 
            program_done = 1;
            return;
            }
        // Take the user to the next set of questions. 
        if (first_choice == 1)
            {
            // Prompt user for second choice. 
            printf("\n");
            printf("Which file you want to process? \n\
Enter 1 to pick the largest file\n\
Enter 2 to pick the smallest file\n\
Enter 3 to specify the name of a file\n\
\n\
Enter a choice from 1 to 3: ");
            // Grab the user's second choice. 
            scanf("%d", &second_choice);
            // Use the Switch to handle each of the three options, then 
            // back to the top of the function if the user has not chosen 
            // to exit. 
            switch(second_choice) 
                {
                case 1:
                    // Process the largest file in the current directory.
                    small_large_files(second_choice);
                    printf("\n");
                    // Once the list is either printed, or there is no movies in
                    // that year, the program will repeat the questions.
                    break; 
                    
                case 2:
                    // Process the smallest csv file in the current directory.
                    small_large_files(second_choice);
                    printf("\n");
                    // Once the list is either printed, or there is no movies in
                    // the file, the program will repeat the questions.
                    break; 

                case 3:
                    // If the user asks to process a specific file:
                    specificFile();
                    printf("\n");
                    // Once the files are printed, or there is no movies in
                    // the file, the program will repeat the questions.
                    break; 

                default: 
                    // If the user provides anything other than an integer 
                    // That is not 1 - 3. 
                    printf("You entered an incorrect choice. Try again.\n");
                    printf("\n");
                    break;
                    }}
        else
            {
            // If the user does not enter the correct choice from the 
            // beginning of the program.
            printf("You entered an incorrect choice. Try again.\n"); 
            printf("\n");
            return;}}}

    






/*
______________________________________________________________________________________________
3)  MAIN FUNCTION 
______________________________________________________________________________________________

3.1) Invalid Handling   - Invalid file processing.
3.2) Run Program        - Run initial function "askQuestions()"
3.3) Exit Program       - Exit program upon User typing initial option "2".
*/


int main(int argc, char *argv[])
    {
    // 3.1)_____________________________________________________________ 
    // First, handle when the user provides more than just the name of 
    // file to run.
    if (argc >= 2)
        {
        printf("You must provide only the name of the file to run\n");
        printf("Example usage: ./movies_by_year\n");
        return EXIT_FAILURE;}




    
    // 3.2)_____________________________________________________________
    // Run the initial questions for the user to choose. 
    askQuestions();
    




    // 3.3)_____________________________________________________________
    // Exit the program once the user finishes processing files. 
    // This can also be done with "return 0;".
    return EXIT_SUCCESS;
    }