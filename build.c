#define BUILD_IMPLEMENTATION
#include "build.h"

int main(int argc, char** argv)
{
    REBUILD_SELF();

    /* Run tests */
    if (RunCommand("cmd.exe /c \"cl.exe /nologo main.c\"") != 0) return 1;
    if (RunCommand("cmd.exe /c \".\\main.exe\" >> test.txt") != 0) return 1;
    
    /* Compile documentation */
    if (RunCommand("cmd.exe /c \"typst compile --diagnostic-format short documentation.typ\"") != 0) return 1;
    return 0;
}
