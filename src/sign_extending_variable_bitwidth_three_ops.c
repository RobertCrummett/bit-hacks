#include <assert.h>

#define CHAR_BIT 8
#define ARRAYSIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

// SNIPPET tables
#define M(B) (1U << ((sizeof(int) * CHAR_BIT) - B)) // CHAR_BIT=bits/byte
static int const multipliers[] =
{
    0,     M(1),  M(2),  M(3),  M(4),  M(5),  M(6),  M(7),
    M(8),  M(9),  M(10), M(11), M(12), M(13), M(14), M(15),
    M(16), M(17), M(18), M(19), M(20), M(21), M(22), M(23),
    M(24), M(25), M(26), M(27), M(28), M(29), M(30), M(31),
    M(32)
}; // (add more if using more than 64 bits)
static int const divisors[] =
{
    1,    ~M(1),  M(2),  M(3),  M(4),  M(5),  M(6),  M(7),
    M(8),  M(9),  M(10), M(11), M(12), M(13), M(14), M(15),
    M(16), M(17), M(18), M(19), M(20), M(21), M(22), M(23),
    M(24), M(25), M(26), M(27), M(28), M(29), M(30), M(31),
    M(32)
}; // (add more for 64 bits)
#undef M
// END

int reference_sign_extend(int x, unsigned int b)
{
    switch (b)
    {
    case 1:  { struct { signed int  v:1; } s; return s.v = x; }
    case 2:  { struct { signed int  v:2; } s; return s.v = x; }
    case 5:  { struct { signed int  v:5; } s; return s.v = x; }
    case 8:  { struct { signed int  v:8; } s; return s.v = x; }
    case 12: { struct { signed int v:12; } s; return s.v = x; }
    case 16: { struct { signed int v:16; } s; return s.v = x; }
    }
    assert(0 && "reference_sign_extend does not support this bit width");
    return 0;
}

int method1(x, b)
{
    int r;
    // SNIPPET 1
    r = (x * multipliers[b]) / divisors[b];
    // END
    return r;
}

// Non portable (relies upon arithmetic right shift keeping sign)
int method2(x, b)
{
    int r;
    // SNIPPET 2
    const int s = -b; // OR:  sizeof(int) * CHAR_BIT - b;
    r = (x << s) >> s;
    // END
    return r;
}

int main(void)
{
    unsigned int widths_to_test[] = {1, 2, 5, 8, 12, 16};
    int num_widths = ARRAYSIZE(widths_to_test);

    for (int w = 0; w < num_widths; w++)
    {
	unsigned int b = widths_to_test[w];

	int min_val = -(1 << (b - 1));
	int max_val = (1 << (b - 1)) - 1;

	int pass_cases[5] = {min_val, -1, 0, 1, max_val};
	if (b == 1) pass_cases[1] = 0; // Make sure to test valid points
	int num_pass = ARRAYSIZE(pass_cases);

	for (int i = 0; i < num_pass; i++)
	{
	    int x = pass_cases[i];
	    int expected = reference_sign_extend(x, b);

	    if (method1(x, b) != expected) return 1;
	    if (method2(x, b) != expected) return 2;
	}
    }
    return 0;
}
