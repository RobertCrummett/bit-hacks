#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#pragma comment(lib, "user32.lib")

#define BUILD_IMPLEMENTATION
#include "build.h"

const char *SourceFilePaths[] =
{
    "compute_the_sign_of_an_integer.c",
    "two_integers_have_opposite_signs.c",
    "integer_absolute_value_without_branching.c",
    "compute_minimum_of_two_integers_without_branching.c",
};

#ifndef BUILD_DIR
#define BUILD_DIR ".build"
#endif // BUILD_DIR

static int BuildAndRun(const char *srcPath)
{
    // The goal of this block is to remove the suffix from the srcPath
    // and build up some other paths from this partial path. Namely, the
    // obj file paths, exe file paths, and pdb/ilk file paths.
    char name[MAX_PATH];
    lstrcpyA(name, srcPath);
    {
        char *ptr = name;
        while (*ptr != '\0') ptr++;
        while (*ptr != '.')  ptr--;
        *ptr = '\0';
    }

    char objPath[MAX_PATH], exePath[MAX_PATH], pdbPath[MAX_PATH];

    // This function relies on the existance of the BUILD_DIR
    // and its subdirectories.
    lstrcpyA(objPath, BUILD_DIR"\\obj\\");
    lstrcatA(objPath, name);
    lstrcatA(objPath, ".obj");

    lstrcpyA(exePath, BUILD_DIR"\\bin\\");
    lstrcatA(exePath, name);
    lstrcatA(exePath, ".exe");

    lstrcpyA(pdbPath, BUILD_DIR"\\bin\\");
    lstrcatA(pdbPath, name);
    lstrcatA(pdbPath, ".pdb");

    // Build the source files.
    char buildCmd[1024];
    wsprintfA(buildCmd,
            "cl.exe /nologo /Zi /Fo:%s /Fe:%s /Fd:%s %s > NUL",
            objPath, exePath, pdbPath, srcPath);
    if (RunCommand(buildCmd) != 0) {
        char msg[1024];
        wsprintfA(msg, "`%s` did not return exit code zero", buildCmd);
        Fail(msg);
        return 1;
    }

    // Run the executables.
    char runCmd[1024];
    wsprintfA(runCmd, ".\\%s", exePath);
    if (RunCommand(runCmd) != 0) {
        char msg[1024];
        wsprintfA(msg, "`%s` did not return exit code zero", runCmd);
        Fail(msg);
        return 1;
    }

    return 0;
}

static int CreateNewDirectoryUnlessItExists(const char *path)
{
    if (CreateDirectoryA(path, NULL) == 0) return 0;

    if (GetLastError() == ERROR_ALREADY_EXISTS)
        return 0; // If the directory exists, no action needs to be taken.

    // In this case, the path specified does not exist.
    char msg[1024];
    wsprintfA(msg, "The path `%s` was not found", path); 
    Fail(msg);
    return 1;
}

int main(int argc, char** argv)
{
    REBUILD_SELF();

    if (CreateNewDirectoryUnlessItExists(BUILD_DIR) != 0)         return 1;
    if (CreateNewDirectoryUnlessItExists(BUILD_DIR "\\bin") != 0) return 1;
    if (CreateNewDirectoryUnlessItExists(BUILD_DIR "\\obj") != 0) return 1;

    int FileCount = sizeof(SourceFilePaths) / sizeof(*SourceFilePaths);
    for (int i = 0; i < FileCount; i++)
    {
        const char *Path = SourceFilePaths[i];
        
        char msg[1024];
        wsprintfA(msg, "[%d/%d] %s", i + 1, FileCount, Path);
        PrintLine(msg);

        if (BuildAndRun(Path) != 0) return 1;
    }

    return 0;
}
