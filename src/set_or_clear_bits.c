#include <stdbool.h>
#include <limits.h>

#define ARRAYSIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

unsigned method1(bool f, unsigned m, unsigned w)
{
    // SNIPPET 1
    w ^= (-f ^ w) & m;
    // END
    return w;
}

unsigned method2(bool f, unsigned m, unsigned w)
{
    // REFERENCE: https://en.wikipedia.org/wiki/Superscalar_processor
    // SNIPPET 2
    // OR, for superscalar CPUs:
    w = (w & ~m) | (-f & m);
    // END
    return w;
}

unsigned branching_set(bool f, unsigned m, unsigned w)
{
    if (f) return w | m;
    else   return w & ~m;
}

int main(void)
{
    unsigned mask_cases[] = {0U, 1U, 2U, 12345U, (1U << 16), UINT_MAX};
    int num_cases = ARRAYSIZE(mask_cases);

    for (int i = 0; i < num_cases; i++) {
	unsigned word = UINT_MAX;
	unsigned mask = mask_cases[i];

	{ // Test case one: flag = true
	    bool flag = true;
	    unsigned expected = branching_set(flag, mask, word);

	    if (method1(flag, mask, word) != expected) return 1;
	    if (method2(flag, mask, word) != expected) return 2;
	}

	{ // Test case two: flag = false
	    bool flag = false;
	    unsigned expected = branching_set(flag, mask, word);

	    if (method1(flag, mask, word) != expected) return 1;
	    if (method2(flag, mask, word) != expected) return 2;
	}
    }

    return 0;
}
