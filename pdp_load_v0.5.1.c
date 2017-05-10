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
#define HAS_XX 1 << 4
int N = 0, Z = 0;

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
    word xx;
}Params;

void load_file (FILE* fin);
void run_program ();
void reg_dump ();
ModeReg get_mr (word w);
word get_nn (word w);
word get_r (word w);
word get_xx (word w);

void change_flags (word w);

void do_halt (Params res);
void do_mov (Params res);
void do_add (Params res);
void do_sob (Params res);
void do_unknown (Params res);
void do_clr (Params res);
void do_beq (Params res);
void do_br (Params res);

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
    printf ("============HALTED==========\n");
    printf ("Regs:\n");    
    for (i = 0; i < 6; i++)
    {
        printf ("\treg[%d]  = %06o\n", i, reg[i]);
    }
    printf ("\treg[SP] = %06o\n", reg[i]);
    printf ("\treg[PC] = %06o\n", reg[++i]);
    printf ("Flags:\n");
    printf ("\tN Z V C\n");
    printf ("\t%d %d - -", N, Z);
    printf ("\n");
}

void run_program ()
{
    int i = 0;
    pc = 01000;
    Params res = {};

    Commands list[] = 
    {
        {0x0FFFF,   0000000, "HALT",    do_halt,    NO_PARAM},
        {0x07000,   0010000, "MOV",     do_mov,     HAS_SS | HAS_DD},
        {0x07000,   0110000, "MOVb",    do_mov,     HAS_SS | HAS_DD},
        {0x0FE00,   0060000, "ADD",     do_add,     HAS_SS | HAS_DD},
        {0x0FE00,   0077000, "SOB",     do_sob,     HAS_R  | HAS_NN},
        {0x0FF00,   0005000, "CLR",     do_clr,     HAS_DD},
        {0x09F00,   0001400, "BEQ",     do_beq,     HAS_XX},
        {0x0FF00,   0000400, "BR",      do_br,      HAS_XX},
        {0x00000,   0000000, "unknown", do_unknown, NO_PARAM}
    };
    
    while (1)
    {
        word w = w_read (pc);

        if ((w & 0x8000) == 0)
            printf ("%06o : %06o ", pc, w);
        else
            printf ("%06o : %06o ", pc, (w & 0x7FFF) | 0x8000);
            
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

                if (cmd.param & HAS_XX)
                    res.xx = get_xx (w);

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
    int b = (w >> 15) & 0x1;

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
            
            if (b == 1)
            {
                if (reg_n != SP || reg_n != PC)
                    reg[reg_n] += 1;
                else
                    reg[reg_n] += 2;
                
                place.val = b_read (place.a);
                
                if ((place.val >> 7) == 1)
                    place.val = place.val | 0xFF00;
            }
            else
            {
                reg[reg_n] += 2;
                place.val = w_read (place.a);
            }

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
            if (b == 1)
            {
                if (reg_n != SP || reg_n != PC)
                    reg[reg_n] -= 1;
                else
                    reg[reg_n] -= 2;
                
                place.a = reg[reg_n];
                place.val = b_read (place.a);
                
                if ((place.val >> 7) == 1)
                    place.val = place.val | 0xFF00;
            }
            else
            {
                reg[reg_n] -= 2;
                place.a = reg[reg_n];
                place.val = w_read (place.a);
            }

            if (reg_n == SP)
                printf ("-(SP) ");
            else if (reg_n == PC)
                printf ("-(PC) ");
            else
                printf ("-(R%d) ", reg_n);
            break;
        case 5:
            place.a = reg[reg_n];
            reg[reg_n] -= 2;
            place.a = w_read (place.a);
            place.val = w_read (place.a);
            printf ("@-(R%d) ", reg_n);
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

word get_xx (word w)
{
    if ((w & 0x80) == 0)
    {
        return w & 0xFF;
    }
    else
        return (-(0x100 - w & 0xFF));
}

void do_halt (Params res)
{
    reg_dump ();
    exit (0);
}

void do_mov (Params res)
{
    reg[res.dd.a] = res.ss.val;
    change_flags (reg[res.dd.a]);
}

void do_add (Params res)
{
    reg[res.dd.a] = res.dd.val + res.ss.val;
    change_flags (reg[res.dd.a]);
}

void do_sob (Params res)
{
    if (--reg[res.r] != 0)
        pc = pc - 2 * res.nn;
}

void do_clr (Params res)
{
    reg[res.dd.a] = 0;
    N = 0;
    Z = 1;
}

void do_beq (Params res)
{
    if (Z == 1)
        do_br (res);
}

void do_br (Params res)
{
    pc = pc + 2 * res.xx;
}

void do_unknown (Params res)
{}

void change_flags (word w)
{
    if ((w & 0xFFFF) == 0)
        Z = 1;
    else
        Z = 0;

    if ((w & 0x8000) == 1)
        N = 1;
    else
        N = 0;
}