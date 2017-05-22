#ifndef HEADERS_H
#define HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>  

#define SP 6
#define PC 7

#define NO_PARAM 0
#define HAS_SS 1
#define HAS_DD 1 << 1
#define HAS_R 1 << 2
#define HAS_NN 1 << 3
#define HAS_XX 1 << 4

#define REG 0
#define MEM 1

#define OSTAT 0xFFFFFF74
#define ODATA 0xFFFFFF76

typedef unsigned char byte;
typedef short int word;
typedef int adr;

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
    int dest;
}ModeReg;

typedef struct Params
{
    ModeReg ss;
    ModeReg dd;
    word nn;
    word r;
    word xx;
}Params;

void b_write (adr a, byte val);
byte b_read (adr a);
void w_write (adr a, word val);
word w_read (adr a);

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
void do_bpl (Params res);
void do_tst (Params res);
void do_jsr (Params res);
void do_rts (Params res);

#endif