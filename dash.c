
// Kristina
// John


#include <stdlib.h>
 #include <sys/wait.h>
 #include <sys/types.h>
 #include <unistd.h>
 #include <string.h>
 #include <stdbool.h>
 #include <stdio.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 
//global, holds path set by user
 char* set_path = "/bin";
 
 //static
 static int single_argument = 1;
 static int log_counter = 0;
 static int controller_loop = 0;
 static int arg_counter = 0; //count argument counter

//main routine
 int main(int argc, char** argv){
 
      char* user_input_command;// entire line of commands in shell(we call it dash) entered by the user
      char** dash_parallel_commands;// parallel commands, each commands is separated by '&' operator
      bool loop_control = true;// control for do-while loop
      bool is_batch = false; //batch mode      
      

      if(argc == 2){
          
          arg_counter = argc;
          printf("%s%d", "I am argc==2. Reading from file.\n");
          single_argument = 0;
          
          int string_list;
          int read_size_from_file;
          FILE *file_handler;  //declaring handler pointer, communication between file and pointer
          
          //fopen() is used to open an existing file
          //"r" read mode, if the file is opened successfully fopen() loads it into memory and sets up a pointer which points to the first character in it
          file_handler = fopen(argv[1], "r");
          
          //fseek(): moving to a specific location in a file
	        //SEEK_END: end of the file
          fseek(file_handler, 0, SEEK_END);
          
          //ftell(): used to find out the position of file pointer in the file with respect to starting of the file.
          string_list = ftell(file_handler);
          
          //rewind():points to the beginning of the file
          rewind(file_handler);
          
          //Allocates size bytes
          user_input_command = (char*) malloc(sizeof(char) * (string_list + 1) );
          
          //fread(): read the entire file
          read_size_from_file = fread(user_input_command, sizeof(char), string_list, file_handler);
          
          //close the open file
          fclose(file_handler);
      
		}
          
      if(argc > 2){
          printf("%s", "Error: Too many arguments, exiting dash shell.\n");
          exit(1); //error 
      }


      int num_of_commands = 0;// count of number of commands as entered by the user and it initilize to zero

      //main loop
      do{
      
          /* Get the entire line */
          char* buffer= NULL;////set buffer to null, address of the first character status where the input string will be stored
          char** commands;//pointer to char pointer of commands
          size_t buffer_size = 0;//size_t: unsigned integral data type, required and returned by the getline()
          
          if(single_argument == 1){
             // printf("I am single argument, value is %i\n");
              printf("dash> "); //single argument in dash shell
              
              
        	    //getline()is used to get the line entered by the user and it takes 3 parameters:
        	    //&input: address of the first character position where the input string will be stored. 
        	    //&bufSize:address of the variable that holds the size of the input buffe.
        	    //stdin: input file handle, and can use getline() to read a line of text from a file
              int result = getline(&user_input_command,&buffer_size,stdin); 
              //printf ("The result in getline is %i\n", result);
          }
          
          //look for exit in commands when enter by the user
          char* check_for_exit = "exit\n";
         
          //strcmp():compares two strings pointed to, by string1 to the string pointed to by string2.
	        //checking whether the command is a built-in command or not.
          if(strcmp(user_input_command,check_for_exit)== 0){
              //printf("%s", "Exit successfully, dash shell terminated.\n");
              exit(0); //gracefully exit
          }
          
          /****Parse the input into constituent pieces, using tokenization********/
          int index = 0; //index for command entered by the user
          char* single_command;// char pointer to single command
          //printf ("I am single command0 %p\n", &single_command);
          commands = malloc(sizeof(char*)); //allocate byte size memory dynamically in heap and set individual to 0
          //printf ("I am commands0 %p\n", &commands);
          
           //strtok():breaks string into a series of tokens the delimiter. 
           //Here tokenize the user_input_command and delimiter is either '&' or newline(\n)
	        //"\r" is carriage return which is used to move the cursor back to the beginning of the line, to overwrite it with new contents
          single_command = strtok(user_input_command,"&\r\n");
          //printf ("I am single command1 %p\n", &single_command);
          
          //check if we hit end of single command
          /**while**/
          while(single_command != NULL){ 
              num_of_commands++; //keep count of number of commands
              //printf ("Number of command is  %i\n", num_of_commands);
              commands[index] = single_command; //fill the commands char** by inserting individual command
              index++;
              
              //realloc(): resizing the allocated memory dynamically previously allocated by the malloc
              commands = realloc(commands,sizeof(char*)*(index+1));
              //printf ("I am commands1 %p\n", &commands);
              single_command = strtok(NULL,"&\r\n"); //set the last index to null, single_command is null
              //printf ("I am single command2 %p\n", &single_command);
          }
          //printf ("Number of commands: %i\n", num_of_commands);
          //keeping counter for number of commands entered by the user
          if(arg_counter != 0){
              if(log_counter == 0){
                  controller_loop  = num_of_commands;
                  log_counter++;
              }
          }
          commands[index] = NULL;//set the last to NULL. string has been fully tokenized since it reached end of the string that is a null character.
          
          char** list_of_commands;// pointer to the list of command_and_argument
          char * command_and_argument;// individual command and its arguments
          int counter = 0;  //counter for individual command and its argument
          int var = 0; //index of commands
          char char_buffer[128]; //size of buffer is 128 bytes
          int status; //
          
          //fork(): creates  a new process by duplicating the calling process.  The new process, referred to
          // as the child, is an exact duplicate of the calling process, referred to as  the  parent
          //printf ("I am list of commands1 %p\n", &list_of_commands);
          //printf ("I am counter %i\n", counter, "less than num of commands %i\n", num_of_commands);
          while(counter < num_of_commands){
              int pid = fork();
              
              //Checking whether pid is child or parent
              if(pid < 0){
                  //print error message             
                  char error_message[30] = "An error has occurred\n";
                  write(STDERR_FILENO, error_message, strlen(error_message));
                  exit(1); //error occured since pid is less than 0 that means it can not create child process        
              
              }
              
              //create child process
              if(pid == 0){
              //printf ("I am child, process id is  %i\n", pid);
              list_of_commands= malloc(sizeof(char*));
              command_and_argument=strtok(commands[counter]," \n\r");// returns NUlL if no more tokens
              //printf ("I am command and argument1 %p\n", &command_and_argument);
              
                  while(command_and_argument != NULL){
                      list_of_commands[var] = command_and_argument;
                      var = var+1;
                      //printf ("I am var and number is %i\n", var);
                      list_of_commands = realloc(list_of_commands,sizeof(char*)*(var+1));
                      //printf ("I am list of commands2 %p\n", &list_of_commands);
                      command_and_argument = strtok(NULL," \n\r");
                      //printf ("I am command and argument2 %p\n", &command_and_argument);                  
                  }                  
                  list_of_commands[var] = NULL;
                 // printf ("I am list of commands3 %p\n", &list_of_commands);
                  
                  /*checking if the command enter in the dash shell is built-in-command or not. If it is, it should not be executed like other programs.*/
             
          	      //cd: cd always take one argument (0 or >1 args should be signaled as an error). To
          	      //change directories, use the chdir()system call with the argument supplied by the
          	      //user; if chdir fails, that is also an error.
           
                  /******cd commmand************/
                  if(strcmp(list_of_commands[0], "cd") == 0){                  
                      int num = chdir(list_of_commands[1]);
                      if(num == -1){
                          //print error message
                            char error_message[30] = "An error has occurred \n";
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            return -1; //returning an error                                                
                      }              
                  
                  loop_control= 1;
                  }
                  
                  /******path command*************/
                  if(strcmp(list_of_commands[0],"path") == 0){
                  
                      if(list_of_commands[1] == NULL){
                  
                          //print error message
                          char error_message[30] = "An error has occurred \n";
                          write(STDERR_FILENO, error_message, strlen(error_message));
                          return -1; //returning an error 
                  
                      }
                      if(list_of_commands[1]!= NULL){
                          
                          //strcpy():function copies the entire string to the destination string
                          strcpy(char_buffer, set_path);
                          //strlen():takes one argument,calculates the length of a given string.
                          int length = strlen(char_buffer);
                          char_buffer[length] = ':';
                          //strcat():takes two arguments, will append a copy of the source string to the end of destination string.
                          strcat(char_buffer, list_of_commands[1]);                         
                      }
                      loop_control = 1;
                  }
                      
                  
                  
          	      //Redirection: sending the output of a program to a file rather than to screen
          	      //will be using ">" character for redirection of standard output
                  
                  /******output redirection********/
                  int fileDescriptor;
                  bool is_redirect = false;
                  char output_file[128]; //size of standard output file is 128 bytes
                  int i;
                  
                  for(i=0;list_of_commands[i]!='\0';i++){
                      if(strcmp(list_of_commands[i],">") == 0){
                          list_of_commands[i] = NULL;
                          strcpy(output_file,list_of_commands[i+1]);
                          is_redirect = true;
                      }
                  }
                  if (is_redirect){
                      int fileDescriptor = creat(output_file, 0644);
                      //copymode 0644
		         //creat: creates a file of the given name if the file does not currently exist and opens the file for writing. 
             //If the file does exist, it merely opens the file for writing.
                      if(fileDescriptor < 0){
                          //print error message
                          char error_message[30] = "An error has occurred \n";
                          write(STDERR_FILENO, error_message, strlen(error_message));
                          exit(1);  //exit the dash shell                   
                      
                      }
                      dup2(fileDescriptor,1);
                      close(fileDescriptor);
                  }
                  
                  execvp(list_of_commands[0],list_of_commands);
                  }
              else{
                    //avoid race condition error
                    while (!(wait(&status) == pid)){
                        //print error message
                        char error_message[30] = "An error has occurred \n";
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    }
                  }
                  counter++;
         }
         free(commands);
       
      }while(loop_control && log_counter == 0 );

        return 0;
 }
