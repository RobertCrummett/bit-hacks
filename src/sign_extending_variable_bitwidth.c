#include <assert.h>
#include <stdbool.h>
#include <limits.h>

#define CHAR_BIT 8
#define ARRAYSIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

int reference_sign_extend(int x, unsigned int b)
{
    switch (b)
    {
    case 2:  { struct { signed int  v:2; } s; return s.v = x; }
    case 5:  { struct { signed int  v:5; } s; return s.v = x; }
    case 8:  { struct { signed int  v:8; } s; return s.v = x; }
    case 12: { struct { signed int v:12; } s; return s.v = x; }
    case 16: { struct { signed int v:16; } s; return s.v = x; }
    }
    assert(0 && "reference_sign_extend does not support this bit width");
    return 0;
}

int method1(int x, unsigned int b)
{
    int r;
    int const m = 1U << (b - 1);
    // BEGIN: method1
    x = x & ((1U << b) - 1);  // skip this if bits in x above position b are already zero
    r = (x ^ m) - m;
    // END: method1
    return r;
}

int method2(int x, unsigned int b)
{
    int r;
    int const m = CHAR_BIT * sizeof(x) - b;
    // BEGIN: method2
    r = (x << m) >> m;
    // END: method2
    return r;
}

int main(void)
{
    unsigned int widths_to_test[] = {2, 5, 8, 12, 16};
    int num_widths = ARRAYSIZE(widths_to_test);

    for (int w = 0; w < num_widths; w++)
    {
	unsigned int b = widths_to_test[w];

	int min_val = -(1 << (b - 1));
	int max_val = (1 << (b - 1)) - 1;

	int pass_cases[5] = {min_val, -1, 0, 1, max_val};
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
