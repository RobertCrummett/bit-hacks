#include <limits.h>

#define ARRAYSIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

/* Sign-extend a 5-bit integer into a 32-bit integer */
int sign_extend_s5(int x)
{
    int r;
    struct { signed int x:5; } s;
    // BEGIN: method1
    r = s.x = x;
    // END: method1
    return r;
}

int main(void) {
    /* Testing sign extension for numbers of constant bit-width = 5
     * The minimum of a signed 5-bit number is -16; the max is 15. 
     * In two's compliment, of course. */

    int pass_cases[5] = {-16, -1, 0, 1, 15};
    int fail_cases[4] = {INT_MIN, -17, 16, INT_MAX};

    int num_pass_cases = ARRAYSIZE(pass_cases);
    for (int i = 0; i < num_pass_cases; i++) {
	int x = pass_cases[i];
	int r = sign_extend_s5(x);
	if (x != r) return 1;
    }

    int num_fail_cases = ARRAYSIZE(fail_cases);
    for (int i = 0; i < num_fail_cases; i++) {
	int x = fail_cases[i];
	int r = sign_extend_s5(x);
	if (x == r) return -1; /* Expected to fail sign extension */
    }

    return 0;
}
