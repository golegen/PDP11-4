#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "pdp_rw.h"

void load_file ();
void mem_dump (adr start, word val);

int main()
{
    load_file ();
    mem_dump (0x040, 0x0004);
}
void load_file ()
{
    adr a = 0;
    int n = 0;
    byte val = 0;    
    
    scanf ("%x", &a);
    scanf ("%x", &n);

    while (n)
    {
        scanf ("%hhx", &val);
        b_write (a, val);
        a = a + 1;
        n--;
    }
}

void mem_dump (adr start, word n)
{
    int i = 0;

    for (i = 0; i < n; i += 2)
        printf ("%06o : %06o\n", start + i, w_read (start + i)); 
}