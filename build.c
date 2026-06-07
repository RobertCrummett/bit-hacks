#define BUILD_IMPLEMENTATION
#include "build.h"

int main(int argc, char** argv)
{
    REBUILD_SELF();

    /* Run tests */
    if (RunCommand("cl.exe /nologo main.c") != 0) return 1;
    if (RunCommand(".\\main.exe >> test.txt") != 0) return 1;
    
    /* Compile documentation */
    if (RunCommand("typst compile --diagnostic-format short documentation.typ") != 0) return 2;
    return 0;
}
