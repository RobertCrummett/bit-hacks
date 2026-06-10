#include <stdbool.h>

static struct { signed   x:5; }   s5;

bool test_all(int input, int output)
{
    // TODO
    return false;
}

int main(void)
{
    int test_cases[] = {-1, 0, 1};
    const int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    unsigned b = 5;
    int r, x = test_cases[i];
    const int m = 1U << (b - 1);

    // BEGIN: method1
    x = x & ((1U << b) - 1);  // skip this if bits in x above position b are already zero
    r = (x ^ m) - m;
    // END: method1



     
    return 0;
}
