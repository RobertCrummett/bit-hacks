#include <limits.h>
#include <stdbool.h>

bool uint_power_of_two(unsigned x) {
    if (x > 1) {
        while (x % 2 == 0) x >>= 1;
    }
    return x == 1;
}

int main(void) {
    unsigned test_cases[] = {0, 1, 2, 3, 63, 64, 65, UINT_MAX};
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_cases; i++) {
        unsigned v = test_cases[i];
        bool f, expected = uint_power_of_two(v);

        // BEGIN: method1
        f = (v & (v - 1)) == 0;
        // END: method1
        if (v == 0) {
            // Expected failure of method1 for v == 0, f should be false, but it is true
            if (f != true) return -1;
        } else {
            if (f != expected) return 1;
        }

        // BEGIN: method2
        f = v && !(v & (v - 1));
        // END: method2
        if (f != expected) return 2;
    }

    return 0;
}
