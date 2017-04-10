#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

enum Commands
{
    HLT =  0,
    ADD = 1,
    SUB = 2,
    MOV = 3,
    IR = 4
};

enum Registers
{
    A = 5,
    B = 6,
    C = 7,
    D = 8
};

void read_commands (unsigned char* regs);

int main()
{
    unsigned char regs[4] = {};

    read_commands (regs);
}

void read_commands (unsigned char* regs)
{
    int i = 0;
    int buf = 0, arg_1 = 0, arg_2 = 0, reg_dest = 0;

    scanf ("%d", &buf);

    while (buf != HLT)
    {

        if (buf == IR)
        {
            for (i = 0; i < 4; i++)
                printf ("%d ", regs[i]);

            printf ("\n");
        }

        if (buf == ADD)
        {
            scanf ("%d", &buf);
            reg_dest = buf - 5;
            arg_1 = regs[reg_dest];

            scanf ("%d", &buf);
            arg_2 = regs[buf - 5];
            regs[reg_dest] = arg_1 + arg_2;
        }

        if (buf == SUB)
        {
            scanf ("%d", &buf);
            reg_dest = buf - 5;
            arg_1 = regs[reg_dest];

            scanf ("%d", &buf);
            arg_2 = regs[buf - 5];
            regs[reg_dest] = arg_1 - arg_2;
        }

        if (buf == MOV)
        {
            scanf ("%d", &buf);
            reg_dest = buf - 5;
            
            scanf ("%d", &buf);
            arg_1 = buf;
            regs[reg_dest] = arg_1;
        }

        scanf ("%d", &buf);
    }
}