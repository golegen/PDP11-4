#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>  

#include "pdp_rw.h"

typedef struct Commands
{
    word mask;
    word opcode;
    char* name;
    void (*do_command)();
}Commands;

void load_file (FILE* fin);
void mem_dump (adr start, word val);
void run_program ();
void do_halt ();
void do_mov ();
void do_add ();
void do_unknown ();


int main(int argc, char* argv[])
{
    FILE* fin = fopen (argv[1], "r");

    if(errno)
	{
		perror("Error");
		exit(1);
	}

    load_file (fin);
    //mem_dump (0x0200, 0x000c);
    run_program ();
    
    fclose (fin);
}
void load_file (FILE* fin)
{
    adr a = 0;
    int n = 0;
    byte val = 0;    
    
    fscanf (fin, "%x", &a);
    fscanf (fin, "%x", &n);

    while (n)
    {
        fscanf (fin, "%hhx", &val);
        b_write (a, val);
        a = a + 1;
        n--;
    }
}

void mem_dump (adr start, word n)
{
    int i = 0;

    for (i = 0; i < n; i += 2)
    {
        printf ("%06o : %06o ", start + i, w_read (start + i));
    }
}

void run_program ()
{
    int pc = 01000;
    int i = 0;

    Commands list[] = 
    {
        {0xFFFF,    0,       "HALT",    do_halt},
        {0170000,   0010000, "MOV",     do_mov},
        {0170000,   0060000, "ADD",     do_add},
        {0,         0,       "unknown", do_unknown}
    };
    
    while (1)
    {
        word w = w_read (pc);
        printf ("%06o : %06o ", pc, w);
        pc += 2;

        for (i = 0; ; i++)
        {
            Commands cmd = list[i];
            if ((w & cmd.mask) == cmd.opcode)
            {
                printf ("%s", cmd.name);
                cmd.do_command ();
                break;
            }
        }
        printf ("\n");
    }
}

void do_halt ()
{
    printf ("\n");
    exit (0);
}

void do_mov ()
{}

void do_add ()
{}

void do_unknown ()
{}