typedef unsigned char byte;
typedef short int word;
typedef int adr;
byte mem[64 * 1024];

void b_write (adr a, byte val);
byte b_read (adr a);
void w_write (adr a, word val);
word w_read (adr a);

void test_mem ();

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

void test_mem()
{
    byte b, be;
    word w, we;

    be = 5;
    b_write (0, be);
    b = b_read (0);
    assert (b == be);

    be = 7;
    b_write (0, be);
    b = b_read (0);
    assert (b == be);

    be = 0xab;
    b_write (0, be);
    b = b_read (0);
    assert (b == be); 

    we = 0x0705;
    b_write (0, 5);
    b_write (1, 7);
    w = w_read(0);
    assert (w == we);

    we = 0xa1b2;
    w_write (0, we);
    w = w_read(0);
    assert (w == we);
}