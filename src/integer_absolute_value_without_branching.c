#include <limits.h>

#define CHAR_BIT 8

unsigned int abs(int x)
{
    return (x < 0) ? -(unsigned)x : x; // With branching
}

int main(void) {
    const int test_cases[] = {-1000, -1, 0, 1, 1000, INT_MAX, INT_MIN};
    const int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_cases; i++) {
        int v = test_cases[i];
        int const mask = v >> sizeof(int) * CHAR_BIT - 1;
        unsigned int r, expected = abs(v);

        // SNIPPET 1
        r = (v + mask) ^ mask;
        // END
        if (r != expected) return 1;

        // SNIPPET 2
        r = (v ^ mask) - mask;
        // END
        if (r != expected) return 2;
    }

    return 0;
}
