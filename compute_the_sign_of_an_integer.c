#include <stdio.h>
#include <limits.h>

#define CHAR_BIT 8

int get_expected_sign(int v) {
    return (v < 0) ? -1 : 0;
}

int main(void) {
    int test_cases[] = {-1000, -1, 0, 1, 1000, INT_MAX, INT_MIN};
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_cases; i++) {
        int v = test_cases[i];
        int expected = get_expected_sign(v);
        int sign;

        // Method 1: Branchless
        // BEGIN: method1
        sign = -(v < 0);
        // END: method1
        if (sign != expected) return 1;

        // Method 2: Flag Register Avoidance
        // BEGIN: method2
        sign = -(int)((unsigned int)((int)v) >> (sizeof(int) * CHAR_BIT - 1));
        // END: method2
        if (sign != expected) return 2;

        // Method 3: Non-portable Right Shift
        // BEGIN: method3
        sign = v >> (sizeof(int) * CHAR_BIT - 1);
        // END: method3
        if (sign != expected) return 3;
    }

    return 0;
}
