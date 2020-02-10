/*
 *Project1
 *Kristina
 *John
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>



//function prototype
char* readFromBatchText(int argc, char** argv);
int executeInDash(char** cmds, int countCommands);

//declaring and initializing
static int singleArgument = 0;
char* setPath = "/bin";

/*
 *main routine
 *argc: number of arguments passed
 *argv: array of character pointers which points to each argument passed to the program
 *
 */

int main(int argc, char** argv){

     char* userInput; //line of input
     //char** commands; //command that user entered
    // int singleArgument;
     bool loopControl  = true;
    
           
     //check if there is any arguments supplied from the command line
     if(argc == 1){
        printf("dash> \n");
	 // singleArgument++;     
     }
     if( argc == 2 ) {
         // printf("The argument supplied is %s\n", argv[1]);
	userInput = readFromBatchText(argc, argv);
    	 printf("%s",userInput); //debugging
    }

     if( argc > 2 ) {
       printf("Error: Too many arguments.\n");
       exit(1); //an error occured so terminates the program
     }

     //while loop, if loopControl is true: goes inside while loop
     while(loopControl){
     
         char* buffer = NULL; //buffer is null, address of the first character position where the input string will be stored
	 char** cmds; //pointer to char pointer of cmds

	 //size_t: unsigned integral data type, required by the getline()
	 size_t bufSize = 0;
         
	 if(singleArgument == 1){
	    printf("dash> "); //single argument in dash shell

	    //getline() will take three arguments:
	    //&input: address of the first character position where the input string will be stored. 
	    //&bufSize:address of the variable that holds the size of the input buffe.
	    //stdin: input file handle, and can use getline() to read a line of text from a file
	    int count = getline(&userInput, &bufSize, stdin);	 
	 }
	 
	 //look for exit in commands when enter by the user
         char* enterExit = "exit\n";

	 //strcmp():compares the string pointed to, by str1 to the string pointed to by str2.
	 //checking whether the command is a built-in command or not.
	 if(strcmp(userInput, enterExit) == 0){
	    kill(0, SIGTERM);
	    exit(0); //comparison between two strings are equal, successfully done 
	 }
         
	 //to parse the input line into constituent pieces, using tokenization
	 int countCommands = 0; //keeping counter for number of commands as entered by the user and it initilize to zero
	 int idx = 0; //index

	 char* token; //char pointer to each token
         cmds = malloc(sizeof(char*)); //allocates byte size in memory dynamically 

	 //strtok():breaks string into a series of tokens using the delimiter. Herre deliminator are &, new line(\n)
	 //"\r" is carriage return which is used to move the cursor back to the beginning of the line, to overwrite it with new contents
	 token = strtok(userInput, "&\r\n"); 
         
	 //check if we hit end of token
	 while(token != NULL){
	 
	      countCommands++; //incrementing number of commands
	      cmds[idx] = token; //inserting each token to fill up the commands char**
	      idx++;
	      //realloc(): resizing the allocated memory dynamically previously allocated by the malloc
	      cmds = realloc(cmds, sizeof(char*) * (idx+1));
	      token = strtok(NULL, "&\r\n");
	  
	 }
	 cmds[idx] = NULL; //string has been fully tokenized since it reached end of the string that is a null character.
         
	 loopControl = executeInDash(cmds, countCommands); //execute when you pass in the commands
	 free(cmds); //cleaning up memory leak

	 if(singleArgument != 1){
	    singleArgument = 1;
	 }
     
     }
			  
     return 0;
}


//function read from file
char* readFromBatchText(int argc, char** argv){

    char *buffer = NULL; //buffer is used to put data in
    FILE *filePointer; //declaring file pointer, communication between file and pointer
    int batchSize;
    int readFileSize;

   // filePointer = fopen("batch.txt","r");

    //fopen() is used to open an existing file
    //"r" read mode, if the file is opened successfully fopen() loads it into memory and sets up a pointer which points to the first character in it
    if((filePointer = fopen(argv[1], "r")) == NULL){
       
       /* print this one and only error message
       *  whenever you encounter an error of any type
       *  The error message should be printed to stderr (standard error)
       */

       char error_message[30] = "An error has occurred\n";
       write(STDERR_FILENO, error_message, strlen(error_message));
       
       //if the shell is invoked with more than one file or if the shell is
       //passed a bad batch file, it should exit by calling exit(1)
       exit(1);
    }
    if(filePointer){

         //fseek(): moving to a specific location in a file
	 //SEEK_END: end of the file
	 fseek(filePointer,0, SEEK_END);

	 //ftell(): used to find out the position of file pointer in the file with respect to starting of the file.
         batchSize = ftell(filePointer);
         
	 //rewind():points to the beginning of the file
	 rewind(filePointer);

	 //Allocates size bytes
	  buffer = (char*) malloc(sizeof(char) * (batchSize + 1));
         
	  //fread(): read the entire file
	  readFileSize = fread(buffer, sizeof(char), batchSize, filePointer);
          
	  if(batchSize != readFileSize){
	     free(buffer);
	     buffer = NULL;
	     exit(0);  
	  
	  }
	  //fclose(): close the file that is being pointed by the filePointer.
	  fclose(filePointer);
        
    }
       return buffer;

}

//function to execute the commands in dash shell
int executeInDash(char** cmds, int countCommands){

   char* commands1; //each command
   char** commands2; //char pointer to pointer of arguments and Command
   char bufferSize[512]; //size of buffer is 512 bytes
   int counter = 0; //counter for commands that are in batch.txt
   int idx = 0; //index
   int position;

   while(counter < countCommands){
       
       //fork(): creates  a new process by duplicating the calling process.  The new process, referred to
       // as the child, is an exact duplicate of the calling process, referred to as  the  parent
       int pid = fork();

       //Checking whether pid is child or parent
       if(pid < 0){
         char error_message[30] = "An error has occurred\n";
	       write(STDERR_FILENO, error_message, strlen(error_message));
         exit(1); //error occured since pid is less than 0 that means it can not create child process        
       }else if(pid == 0){ //child process is successfully created
          commands2 = malloc(sizeof(char*));
	        commands1 = strtok(cmds[counter], " \n\r"); //returns null
	      
	      while(commands1 != NULL){
	         commands2[idx] = commands1; //fill the char
		       idx = idx+1;
		       commands2 = realloc(commands2, sizeof(char*) * (idx + 1));
           commands1 = strtok(NULL, " \n\r"); //end of tokens
	               
	      }
        commands2[idx] = NULL; //set the last index to null

	      /*checking if the command enter in the dash shell is built-in-command or not. If it is, it should not be executed like other programs.*/
             
	      //cd: cd always take one argument (0 or >1 args should be signaled as an error). To
	      //change directories, use the chdir()system call with the argument supplied by the
	      //user; if chdir fails, that is also an error.
             
	      if(strcmp(commands2[0], "cd") == 0){
           int signal = chdir(commands2[1]);
		       //if signal fails then an error occured
           if(signal == -1){
		           char error_message[30] = "An error has occurred\n";
		           write(STDERR_FILENO, error_message, strlen(error_message));
		           exit(1); //if signal is less than -1, error and exit
           }         
		       return 1;	           
	      }

	      // checking whether the path command is a built-in-command or not
	      if(strcmp(commands2[0], "path") == 0){
	         if(commands2[1] != NULL){
		    
		            //strcpy():function copies the entire string to the destination string
		            strcpy(bufferSize, setPath);
		            //strlen():takes one argument,calculates the length of a given string.
		            int length = strlen(bufferSize);
                bufferSize[length] = ':';
		            //strcat():takes two arguments, will append a copy of the source string to the end of destination string.
		            strcat(bufferSize, commands2[1]);			 
		       }
		      return 1;      
	      }

	      //Redirection: sending the output of a program to a file rather than to screen
	      //will be using ">" character for redirection of standard output
	      int redirectFile;
	      bool redirectBool = false;
	      char sizeOutput[512]; //512 bytes
	      int i;
        
        //for loop
	      for(i = 0; commands2[i] != '\0'; i++){
	         if(strcmp(commands2[i], ">") == 0){
		           commands2[i] = NULL;
		           strcpy(sizeOutput, commands2[i+1]);
               redirectBool = true;		 
		 
		        }

	       }	 
	         
		     if(redirectBool){
		         int redirectFile;
		         //print error message whenever encounter an error of any type
		         //copymode 0644
		         //creat: creates a file of the given name if the file does not currently exist and opens the file for writing. 
             //If the file does exist, it merely opens the file for writing.
             if((redirectFile = creat(sizeOutput, 0644)) < 0){
		             //print the error message
		             char error_message[30] = "An error has occurred\n";
		             write(STDERR_FILENO, error_message, strlen(error_message));
		             exit(0); //exit the dash shell	    
		          }
		    
		          //The dup2() system call performs the same task as dup(), but instead  of using the lowest-numbered unused file descriptor, it uses the file
		          //descriptor number specified in newfd.  If the file descriptor newfd was previously open, it is silently closed before being reused.
		    
		          dup2(redirectFile, 1);
		          close(redirectFile); //close it	 
		 
           }
                 //print error message
		       if((execvp(commands2[0], commands2)) == -1){
		           char error_message[30] = "An error has occurred\n";
		           write(STDERR_FILENO, error_message, strlen(error_message));
		 
		       } 
		 
		       }else{
		 
		           while(!(wait(&position) == pid)){		       
		              //print error meaasge
			            char error_message[30] = "An error has occurred\n";
			            write(STDERR_FILENO, error_message, strlen(error_message));		       
		            }			 
		       }
		       counter++;
	      
	         }
           return 1; 
       
       }
   
  







