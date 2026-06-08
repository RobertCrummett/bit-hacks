#include <limits.h>

#define CHAR_BIT 8

int get_expected_sign(int v) {
    return (v < 0) ? -1 : 0;
}

int main(void) {
    const int test_cases[] = {-1000, -1, 0, 1, 1000, INT_MAX, INT_MIN};
    const int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_cases; i++) {
        int sign, v = test_cases[i], expected = get_expected_sign(v);

        // BEGIN: method1
        // CHAR_BIT is the number of bits per byte (normally 8).
        sign = -(v < 0);
        // END: method1
        if (sign != expected) return 1;

        // BEGIN: method2
        // or, to avoid branching on CPUs with flag registers (IA32).
        sign = -(int)((unsigned int)((int)v) >> (sizeof(int) * CHAR_BIT - 1));
        // END: method2
        if (sign != expected) return 2;

        // BEGIN: method3
        // or, for one less instruction (but not portable):
        sign = v >> (sizeof(int) * CHAR_BIT - 1);
        // END: method3
        if (sign != expected) return 3;
    }

    return 0;
}
