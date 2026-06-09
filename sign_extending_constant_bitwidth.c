int main(void) {
    const int test_cases[] = {-3, -1, 0, 1, 3, 4};
    const int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    struct { signed int x:5; } s;
    
    for (int i = 0; i < num_cases; i++) {
        int r, x = test_cases[i];

        // BEGIN: method1
        r = s.x = x;
        // END: method1
        if (x != r) return 1;
    }

    return 0;
}
