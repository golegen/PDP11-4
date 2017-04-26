#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>  

#include "pdp_rw.h"

#define NO_PARAM 0
#define HAS_SS 1
#define HAS_DD 1<<1
#define SS 6
#define DD 0

typedef struct Commands
{
    word mask;
    word opcode;
    char* name;
    void (*do_command)();
    word param;
}Commands;

typedef struct ModeReg 
{
    adr a;
    word val;
}ModeReg;

typedef struct SSDD
{
    ModeReg ss;
    ModeReg dd;
}SSDD;

void load_file (FILE* fin);
void run_program ();
void reg_dump ();
ModeReg get_mr (word w, int n);

void do_halt (SSDD);
void do_mov (SSDD);
void do_add (SSDD);
void do_unknown (SSDD);

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

void reg_dump ()
{
    int i = 0;
    printf ("\n");
    printf ("=========================\n");
    printf ("Regs:\n");    
    for (i = 0; i < 6; i++)
    {
        printf ("\treg[%d]  = %06o\n", i, reg[i]);
    }
    printf ("\treg[SP] = %06o\n", reg[i]);
    printf ("\treg[PC] = %06o\n", reg[++i]);
    printf ("\n");
}

void run_program ()
{
    int pc = 01000;
    int i = 0;
    SSDD res = {};

    Commands list[] = 
    {
        {0xFFFF,    0,       "HALT",    do_halt,    NO_PARAM},
        {0170000,   0010000, "MOV",     do_mov,     HAS_SS | HAS_DD},
        {0170000,   0060000, "ADD",     do_add,     HAS_SS | HAS_DD},
        {0,         0,       "unknown", do_unknown, NO_PARAM}
    };
    
    while (1)
    {
        word w = w_read (pc);
        printf ("%06o : %06o ", pc, w);
        pc += 2;
        reg[7] = pc;

        for (i = 0; ; i++)
        {
            Commands cmd = list[i];
            if ((w & cmd.mask) == cmd.opcode)
            {
                printf ("%s ", cmd.name);
                switch (cmd.param)
                {
                    case NO_PARAM:
                        break;

                    case HAS_DD:
                        res.dd = get_mr (w, DD);
                        break;

                    case HAS_SS:
                        res.ss = get_mr (w, SS);
                        break;

                    case HAS_SS | HAS_DD:
                        res.ss = get_mr (w, SS);
                        res.dd = get_mr (w, DD);
                        break;
                }
                cmd.do_command (res);
                break;
            }
        }
        printf ("\n");
    }
}

ModeReg get_mr (word w, int n)
{
    ModeReg place;

    int reg_n = (w >> n) & 7;
    int mode = (w >> (n + 3)) & 7;

    switch (mode)
    {
        case 0:
            place.a = reg_n;
            place.val = reg[reg_n];
            printf ("R%d ", reg_n);
            break;

        case 1:
            break;
        
        case 2:
        if (reg_n == 7)
            {
                place.a = reg[reg_n];
                place.val = w_read (place.a);
                printf ("#%d ", place.val);
            }
            else
            {
                place.a = reg[reg_n];
                place.val = w_read (place.a);
                printf ("(R%d) ", reg_n);
            }
            break;

        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;

        case 7:
            break;
    }

    return place;
}

void do_halt (SSDD res)
{
    reg_dump ();
    exit (0);
}

void do_mov (SSDD res)
{
    reg[res.dd.a] = res.ss.val;
}

void do_add (SSDD res)
{
    reg[res.dd.a] = res.dd.val + res.ss.val;
}

void do_unknown (SSDD res)
{}