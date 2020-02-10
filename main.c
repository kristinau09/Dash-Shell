// Kristina Upadhaya
// John D'Angelo

#include "main.h"


int main(int argc, char** argv)
{
    printf("Hello world!\n");

    // Interactive vs batch mode
    // Batch mode
    if(argc > 1)
    {
        // Open second argument as input file
        //inputfile.open(argv[1]);
        //TODO

        // File failed to open
        if(0 /*!inputfile*/)
        {
            fprintf(stderr, "Could not open batch file, \"%s\", please try again.\n", argv[1]);
            exit(1);
        }
    }


    // Main program loop
    do
    {
        // Get next line to parse
        if(0)    // Batch mode
        {

        }
        else    // Interactive mode
        {

        }

        // Parse input line
        parse();

        // Execute proper command
        // Built-in commands (exit, cd, path)
        //TODO

        // Regular commands
        //TODO

    } while(0/*not EOF*/);
    // EOF reached; return and exit() are the same


    return 0;
}
