#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#pragma comment(lib, "user32.lib")

#define BUILD_IMPLEMENTATION
#include "build.h"

const char *SourceFilePaths[] =
{
    "sign_of_an_integer.c",
    "two_integers_have_opposite_signs.c",
    "integer_absolute_value_without_branching.c",
    "minimum_of_two_integers_without_branching.c",
    "integer_power_of_two.c",
    "sign_extending_constant_bitwidth.c",
    "sign_extending_variable_bitwidth.c",
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
    if (RunCommand(buildCmd) != 0) return 1; // Exit code 1 indicates build failed

    // Run the executables.
    char runCmd[1024];
    wsprintfA(runCmd, ".\\%s", exePath);
    if (RunCommand(runCmd) != 0) return 2; // Exit code 2 indicates run failed

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

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) {
	    Fail("Standard output handle is invalid");
	    return 1;
    }

    int FileCount = sizeof(SourceFilePaths) / sizeof(*SourceFilePaths);

    for (int i = 0; i < FileCount; i++)
    {
        const char *Path = SourceFilePaths[i];
        int ExitCode = BuildAndRun(Path);
        
        // Logging result to the standard output
        char msg[1024];
        wsprintfA(msg, "[%d/%d] ", i + 1, FileCount);
        Print(hStdOut, msg);

        DWORD ResetWhite = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED;
        switch (ExitCode) {
	    case 0: // Compiled and ran successfully
		SetConsoleTextAttribute(hStdOut, FOREGROUND_GREEN);
		Print(hStdOut, "PASS");
		SetConsoleTextAttribute(hStdOut, ResetWhite);
		break;
	    case 1: // Failed to compile
		SetConsoleTextAttribute(hStdOut, FOREGROUND_RED);
		Print(hStdOut, "FAIL");
		SetConsoleTextAttribute(hStdOut, ResetWhite);
		break;
	    case 2: // Compiled but did not return expected exit code
		SetConsoleTextAttribute(hStdOut, FOREGROUND_RED|FOREGROUND_GREEN);
		Print(hStdOut, "FAIL");
		SetConsoleTextAttribute(hStdOut, ResetWhite);
		break;
	    default:
		Fail("Unknown exit code returned from BuildAndRun");
		break;
	}

        wsprintfA(msg, " %s\n", Path);
        Print(hStdOut, msg);
    }

    return 0;
}
