#include "headers.h"

byte mem[64 * 1024];

void b_write (adr a, byte val)
{
    mem[a] = val;
}

byte b_read (adr a)
{
    return mem[a];
}

void w_write (adr a, word val)
{
    mem[a] = val & 0xFF;
    mem[a + 1] = (val >> 8) & 0xFF; 
}

word w_read (adr a)
{
    word val;
    val = mem[a + 1];
    val = val << 8;
    val = val | mem[a];
    return val;    
}
