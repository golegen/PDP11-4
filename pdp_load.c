#include "headers.h"

int main(int argc, char* argv[])
{
    FILE* fin = fopen (argv[1], "r");

    if(errno)
	{
		perror("Error");
		exit(1);
	}

    load_file (fin);
    fclose (fin);
    
    run_program ();
}
