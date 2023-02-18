/*
    Name:           Kristina Montanez
    Date:           October 11, 2021
    Class:          CS344
    Project:        Assignment 1
    Description:    Program to read a CSV file with movie data and process data into structs 
                    for each movie. The structs are then used to create linked lists. The user 
                    can then choose from 4 options to view movies. The program then prints out
                    the data about the movies based on the user's choice. 
______________________________________________________________________________________________
*/

//Include the usual headers. 
//include GNU option just in case.
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
/*

______________________________________________________________________________________________
TABLE OF CONTENTS:

1) STRUCTS FOR MOVIES:

    1.1) Struct movie       - struct to organize movie information.
    1.2) Struct createMovie - to parse information from each line.
    1.3) Struct processFile - to process the file for the linked list.

2) FUNCTIONS:

    2.1) printMovie()       - Testing function to show all movies.
    2.2) printMovieList()   - Support for testing function.
    2.3) printYear()        - Function to print out every movie made in a given year.
    2.4) printRatings()     - Function to print out the highest rated movie in each year.
    2.5) printLanguage()    - Function to print out all movies in a certain language.
    2.6) askQuestions()     - Function for initial and follow up questions.

3) MAIN FUNCTION:

    3.1) Invalid Handling   - invalid file processing
    3.2) Process File       - create movie list with processFile()
    3.3) Notify Success     - Display valid file processing
    3.4) Run Program        - Run initial function "askQuestions()"
    3.5) Exit Program       - Exit program upon User typing option "4".











______________________________________________________________________________________________
______________________________________________________________________________________________
1)  STRUCTS FOR MOVIES
______________________________________________________________________________________________
1.1) Struct movie       - movie information.
1.2) Struct createMovie - to parse information from each line.
1.3) Struct processFile - to process the file for the linked list.
*/




// 1.1)_____________________________________________________________
// Struct for movie information. The information will parsed, and 
// will have title, year, languages, and ratingValue for each movie.
struct movie 
    {
    char *title;
    char *year;
    char *languages;
    float ratingValue;
    struct movie *next;
    };




// 1.2)_____________________________________________________________ 
// Parse the current line which is comma delimited and create a
// movie struct with the data in this line. This code is taken from
// the "students" demo provided in class, as assignment instructions
// allow for. 
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
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->year, token);

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
// allow for. 
struct movie *processFile(char *filePath, int *number_of_movies)
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

        //Add to the count of how many movies there are. 
        *number_of_movies = *number_of_movies + 1;   
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
2.1) printMovie()       - Testing function to show all movies.
2.2) printMovieList()   - Support for testing function.
2.3) printYear()        - Function to print out every movie made in a given year.
2.4) printRatings()     - Function to print out the highest rated movie in each year.
2.5) printLanguage()    - Function to print out all movies in a certain language.
2.6) askQuestions()     - Function for initial and follow up questions.
*/







// 2.1)_____________________________________________________________
//Helper function for printMovieList(). Used for testing to see if 
//the .csv file has loaded properly. Prints each movie out by title,
//language, year, and raiting value. This fuction is provided by  
//the "students" demo provided in class, as assignment instructions
// allow for.
void printMovie(struct movie* a_movie)
    {
    printf("%s %s %s %.1f\n", a_movie->title,
                              a_movie->languages,
                              a_movie->year,
                              a_movie->ratingValue);}








// 2.2)_____________________________________________________________
// Testing function to print out all movies in the .csv file. Used 
// to make sure that all movies have been added to the linked list. 
// This fuction is provided by the "students" demo provided in class, 
// as assignment instructions allow for.
void printMovieList(struct movie *list)
    {
    while (list != NULL)
        {
        printMovie(list);
        list = list->next;}}








// 2.3)_____________________________________________________________
// Function to print out all movies in a given year. If the year that
// the user provides does not have a movie listed, then it will tell
// the user that no data is available.
void printYear(struct movie *list, int movie_year) 
    {
    // "no_movies" will determine if any movies were released in 
    // a specified year.
    int no_movies = 0;
    // check through the entire list.
    while (list != NULL)
        {
        // change our string into a integer for comparing to
        // movie_year integer. 
        int current_year = atoi(list->year);
        if (current_year == movie_year)
            {
            // print out the title of the movie.
            printf("%s\n", list->title);
            // update our "no_movies" to 1, to establish that
            // we have at least found one movie. 
            no_movies = 1;
            }
        // move to the next movie. 
        list = list->next;}
    // if no movies are found for that year, then tell the user
    // that no data was found, and send the user back to the 
    // intial questions. 
    if (no_movies == 0) 
        {
        printf("No data about movies released in the year %d\n", movie_year);
        }}







// 2.4)_____________________________________________________________
// Function to print out the highest rated movie of each year. If 
// there is a tie, the program will display any one movie that had 
// the highest rating that year. In going through each year in 
// chronological order, the results will provide the highest ratings
// in order.
void printRatings(struct movie *list)
    { 
    float highest_rating;  
    // Keep track of highest rated movie for each year.
    struct movie *highestRatedYear = malloc(sizeof(struct movie)); 
    // Keep track of the beginning so the list can be reused
    struct movie *beginning = list; 
    // set our first title to NULL for printing later.
    highestRatedYear->title = NULL;
    // For each year, run through the list 1900-2022.
    for (int year_range = 1900; year_range < 2022; year_range++) 
        {
        //start out with the comparison at 0.
        highest_rating = 0.0;
        // Inner loop for adding to new struct.
        while (list != NULL)
            {
            //change year to an int to use for comparison.
            int list_year = atoi(list->year);
            // If we find a movie in the same year, with a higher rating,
            // we copy it to the new struct.
            if (year_range == list_year && list->ratingValue > highest_rating)
                {
                //copy the information just like in the "students" demo code.
                highestRatedYear->title = calloc(strlen(list->title) + 1, sizeof(char));
                strcpy(highestRatedYear->title, list->title);
                //copy the information for the year from the origingal string. 
                highestRatedYear->year = calloc(strlen(list->year) + 1, sizeof(char));
                strcpy(highestRatedYear->year, list->year);
                //copy the information for the language so we have a placeholder.
                highestRatedYear->languages = calloc(strlen(list->languages) + 1, sizeof(char));
                strcpy(highestRatedYear->languages, list->languages);
                //copy over the ratingValue.
                highestRatedYear->ratingValue = list->ratingValue;
                // Set the next node to NULL in the newly created movie entry
                highestRatedYear->next = NULL;
                // and update the max rating
                highest_rating = list->ratingValue;
                }
            list = list->next;
            }
        // Don't print info for years without any movies
        if (highestRatedYear->title != NULL) 
            {
            printf("%s %.1f %s\n", highestRatedYear->year, 
                                   highestRatedYear->ratingValue, 
                                   highestRatedYear->title);
            }
        // Reset the list for the outer loop. 
        highestRatedYear->title = NULL;
        list = beginning;
        } 
        free(highestRatedYear);
        free(beginning);
        }








// 2.5)_____________________________________________________________
// Function to print out the all movies in the specific language 
// provided by the user. If there are no movies in that language, 
// the program will notify the user and send the program back to 
// the initial questions. 
void printLanguage(struct movie *list, char *language) 
    {
    // "no_movies" will determine if any movies were released in 
    // a specified year. 
    int no_movies = 0;
    // Start our loop through the list.
    while (list != NULL)
        {
        // We create a reference to the languages just like in the "students" demo.
        char *newLanguage = calloc(strlen(list->languages) + 1, sizeof(char));
        strcpy(newLanguage, list->languages);
        // create a throwaway pointer for chosen language.
        char *ptr;
        // parse the languages and take out the brackets. 
        char *parse = strtok_r(newLanguage, "];[", &ptr);
        // Go through our list.
        while (parse != NULL)
            {
            // Look at the first parsed language compared to 
            // the selected language.
            if (strcmp(parse, language) == 0)
                {
                no_movies = 1;
                printf("%s %s\n", list->year, 
                                  list->title);
                }
            // create a throwaway pointer for chosen language.
            char *ptr;
            // Move up one. 
            parse = strtok_r(NULL, "];[", &ptr);
            }
        // Move to the next movie.
        list = list->next;
        // free our parser.
        free(parse);
        // free our reference to the language.
        free(newLanguage);
        }
    // When there are no movies, we can print out 
    // to the user that there is no movies in that language.
    if (no_movies == 0)
        {
        printf("No data provided for any movies released with the language %s.\n", language);
        }
    }
    







// 2.6)_____________________________________________________________
// Function to start the initial questions for the user, asking which
// movies they would like to see. The function uses a switch for each
// option, and will call on itself if the user enters invalid input or
// is provided a movie list, and has not chosen to exit the program.
void askQuestions(struct movie *list)
    {
    // Variables to hold the initial and subsequent choices from user.
    int first_choice;
    int movie_year;
    char language[21]; 

// Print out the instructions for the user's initial choice. 
printf("1. Show movies released in the specified year \n\
2. Show highest rated movie for each year \n\
3. Show the title and year of release of all movies in a specific language \n\
4. Exit from the program \n\
\n\
Enter a choice from 1 to 4: ");

    // Grab the user's initial input. 
    scanf("%d", &first_choice);

    // Use the Switch to handle each of the four options, calling on 
    // askQuestions() when the user has not chosen to exit. 
    switch(first_choice) 
        {
        case 1:
            printf("Enter the year for which you want to see movies: ");
            scanf("%d", &movie_year);
            // Check if the input date has any movies in that year. 
            printYear(list, movie_year);
            printf("\n");
            // Once the list is either printed, or there is no movies in
            // that year, the program will repeat the questions.
            askQuestions(list);
            break; 
            
        case 2:
            // Do the movie list for highest rating each year. 
            printRatings(list);
            printf("\n");
            // Once the list is either printed, or there is no movies in
            // the file, the program will repeat the questions.
            askQuestions(list);
            break; 

        case 3:
            // If the user asks for all movies in a certain language:
            printf("Enter the language for which you want to see movies: ");
            // Scan 20 chars long (the 21st is saved for the '\0')
            scanf("%20s", language);
            // Check to see if any movies are in the given language. 
            printLanguage(list, language);
            printf("\n");
            // Once the list is either printed, or there is no movies in
            // the file, the program will repeat the questions.
            askQuestions(list);
            break; 

        case 4:
            // If the user choses to exit the program, break and move to 
            // End of main function.
            break; 
        
        default: 
            // If the user provides anything other than an integer 
            // That is not 1 - 4. 
            printf("You entered an incorrect choice. Try again.\n");
            printf("\n");
            askQuestions(list);}}










/*
______________________________________________________________________________________________
3)  MAIN FUNCTION 
______________________________________________________________________________________________

3.1) Invalid Handling   - invalid file processing
3.2) Process File       - create movie list with processFile()
3.3) Notify Success     - Display valid file processing
3.4) Run Program        - Run initial function "askQuestions()"
3.5) Exit Program       - Exit program upon User typing option "4".
*/


int main(int argc, char *argv[])
    {

    // 3.1)_____________________________________________________________ 
    // First, handle when the user does not first process 
    // the csv file. Compile the program as follows:
    // gcc --std=gnu99 -o movies main.c
    if (argc < 2)
        {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movies_sample_1.csv\n");
        return EXIT_FAILURE;
        }





    // 3.2)_____________________________________________________________
    // Start our counter. 
    int number_of_movies = 0;
    // Process the file provided as an argument to the program to
    // create a linked list of movie structs and print out the list.
    struct movie *list = processFile(argv[1], &number_of_movies);
    
    




    // 3.3)_____________________________________________________________ 
    // After we are done, tell the user the file processed successfully.
    printf("Processed file %s and parsed data for %d movies\n", argv[1], number_of_movies - 1);
    printf("\n");
    
    



    
    // 3.4)_____________________________________________________________
    // Run the initial questions for the user to choose. 
    askQuestions(list);
    





    // 3.5)_____________________________________________________________
    // Free memory created for struct.
    // Exit the program once the user finishes looking at movies. 
    // This can also be done with "return 0;".
    free(list);
    return EXIT_SUCCESS;
    }