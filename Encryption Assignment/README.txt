Name:           Kristina Montanez
Date:           Nov 29, 2021
Class:          CS344
Project:        Assignment 5
Description:    Program that will encrypt and decrypt plaintext into "ciphertext", using a 
                key, with modulo 27 operations-The 27 characters are the 26 capital letters, 
                and the space character. All 27 characters will be encrypted and decrypted.
                The program consists of 5 small programs in C. 2 programs function as servers,
                and 2 will be clients. The clients will use the servers to perform work, and 
                the 5th program is a standalone keygen file. 

                Example code provided for assignment: 
                "client.c"
                "server.c"
                "compileall.sh"
                "p5testscript"

__________________________________________________________________________________________
README TEXT FILE: 



The provided zip folder named "montanek_program5.zip" will have 13 files including: 

1.  compileall.sh
2.  enc_server.c
3.  enc_client.c
4.  dec_server.c
5.  dec_client.c
6.  keygen.c
7.  p5testscript
8.  plaintext1.txt
9.  plaintext2.txt
10  plaintext3.txt
11. plaintext4.txt
12. plaintext5.txt
13. README.txt (this file)

Upon zip extraction, the files will be available in the 
montanek/CS344/vsprojects/Assignment\5/ directory.
Directions for compiling line_processor.c are as follows: 

1. chmod 777 compileall.sh
2. ./compileall.sh
3. chmod +x ./p5testscript
4. ./p5testscript RANDOM_PORT1 RANDOM_PORT2 > mytestresults 2>&1

* PLEASE NOTE: the "RANDOM_PORT1" and "RANDOM_PORT2" are filled with numbers.

The following execution uses the GNU99 compiler, and each file contains the
"#define _GNU_SOURCE" for ease of use if the above command line for gnu99 
is not used. 


