#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>  

#include "pdp_rw.h"

#define SP 6
#define PC 7
#define sp reg[6]
#define pc reg[7]

#define NO_PARAM 0
#define HAS_SS 1
#define HAS_DD 1 << 1
#define HAS_R 1 << 2
#define HAS_NN 1 << 3

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

typedef struct Params
{
    ModeReg ss;
    ModeReg dd;
    word nn;
    word r;
}Params;

void load_file (FILE* fin);
void run_program ();
void reg_dump ();
ModeReg get_mr (word w);
word get_nn (word w);
word get_r (word w);


void do_halt (Params res);
void do_mov (Params res);
void do_add (Params res);
void do_sob (Params res);
void do_unknown (Params res);
void do_clr (Params res);

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
    
    while (fscanf (fin, "%x %x", &a, &n) == 2)
    {
        while (n)
        {
            fscanf (fin, "%hhx", &val);
            b_write (a, val);
            a = a + 1;
            n--;
        }
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
    int i = 0;
    pc = 01000;
    Params res = {};

    Commands list[] = 
    {
        {0xFFFF,    0,       "HALT",    do_halt,    NO_PARAM},
        {0170000,   0010000, "MOV",     do_mov,     HAS_SS | HAS_DD},
        {0170000,   0060000, "ADD",     do_add,     HAS_SS | HAS_DD},
        {0177000,   0077000, "SOB",     do_sob,     HAS_R | HAS_NN},
        {0xFF00,    005000,  "CLR",     do_clr,     HAS_DD},
        {0,         0,       "unknown", do_unknown, NO_PARAM}
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
                printf ("%s ", cmd.name);

                if (cmd.param & HAS_SS)
                    res.ss = get_mr (w >> 6);

                if (cmd.param & HAS_DD)
                    res.dd = get_mr (w);

                if (cmd.param & HAS_NN)
                    res.nn = get_nn (w);

                if (cmd.param & HAS_R)
                    res.r = get_r (w);

                cmd.do_command (res);
                break;
            }
        }
        printf ("\n");
    }
}

ModeReg get_mr (word w)
{
    ModeReg place;

    int reg_n = w & 7;
    int mode = (w >> 3) & 7;

    switch (mode)
    {
        case 0:
            place.a = reg_n;
            place.val = reg[reg_n];
            printf ("R%d ", reg_n);
            break;

        case 1:
            place.a = reg[reg_n];
            place.val = w_read (place.a);
            printf ("(R%d) ", reg_n);
            break;
        
        case 2:
            place.a = reg[reg_n];
            reg[reg_n] += 2;
            place.val = w_read (place.a);

            if (reg_n == PC)
                printf ("#%d ", place.val);
            else
                printf ("(R%d)+ ", reg_n);
            break;

        case 3:
            place.a = reg[reg_n];
            reg[reg_n] += 2;
            place.a = w_read (place.a);
            place.val = w_read (place.a);

            if (reg_n == PC)
                printf ("@#%d ", place.val);
            else
                printf ("@(R%d)+ ", reg_n);
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

word get_r (word w)
{
    word r = (w >> 6) & 7;
    printf ("R%d", r);
    return r;
}

word get_nn (word w)
{
    return w & 0x3F;
}

void do_halt (Params res)
{
    reg_dump ();
    exit (0);
}

void do_mov (Params res)
{
    reg[res.dd.a] = res.ss.val;
}

void do_add (Params res)
{
    reg[res.dd.a] = res.dd.val + res.ss.val;
}

void do_sob (Params res)
{
    if (--reg[res.r] != 0)
    {
        pc = pc - 2 * res.nn;
    }
}

void do_clr (Params res)
{
    reg[res.dd.a] = 0;
}

void do_unknown (Params res)
{}