#include <stdbool.h>

typedef struct {
    int x, y;
} PairI32;

bool opposite_signs(int x, int y)
{
    if ((x >= 0) && (y < 0)) return true;
    if ((x < 0) && (y >= 0)) return true;
    return false;
}

int main(void) {
    const PairI32 test_cases[] = {
        {-1, 1}, {1, -1}, {1, 1}, {-1, -1},
        {1, 0}, {0, 1}, {-1, 0}, {0, -1},
        {0, 0},
    };
    const int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_cases; i++)
    {
        PairI32 pair = test_cases[i];
        int x = pair.x;
        int y = pair.y;

        // BEGIN: method1
        bool f = ((x ^ y) < 0); // true iff x and y have opposite signs
        // END: method1
        bool expected = opposite_signs(x, y);
        if (f != expected) return 1;
    }

    return 0;
}
