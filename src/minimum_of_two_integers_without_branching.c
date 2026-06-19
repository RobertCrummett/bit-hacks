#include <limits.h>

#define CHAR_BIT 8

typedef struct {
    int x, y;
} PairI32;

int min(int x, int y)
{
    return (x < y) ? x : y;
}

int main(void)
{
    const PairI32 test_cases[] = 
    {
	{-1, -1}, {0, 0}, {1, 1}, {0, 1}, {0, -1},
	{INT_MAX, 1}, {INT_MIN, -1}, {-1, INT_MIN}, {-1, INT_MAX},
	// method2 is not expected to pass the following cases
	{INT_MIN, INT_MAX}, {INT_MAX, INT_MIN}, {INT_MAX, -1}, {-1, INT_MIN}
    };
    const int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_cases; i++)
    {
	const PairI32 pair = test_cases[i];
        int x = pair.x, y = pair.y, r;
        int expected = min(x, y);

        // SNIPPET 1
        r = y ^ ((x ^ y) & -(x < y)); // min(x, y)	
        // END
        if (expected != r) return 1;
        
        // The method below is only valid when INT_MIN <= x - y <= INT_MAX
        if (i < 9)
	{
            // SNIPPET 2
            r = y + ((x - y) & ((x - y) >> (sizeof(int) * CHAR_BIT - 1))); // min(x, y)
            // END
            if (expected != r) return 2;
        }
    }

    return 0;
}
