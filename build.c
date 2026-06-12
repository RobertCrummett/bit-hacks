#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma comment(lib, "user32.lib")

#define BUILD_IMPLEMENTATION
#include "build.h"

const char *SourceFilePaths[] =
{
    "src/sign_of_an_integer.c",
    "src/two_integers_have_opposite_signs.c",
    "src/integer_absolute_value_without_branching.c",
    "src/minimum_of_two_integers_without_branching.c",
    "src/integer_power_of_two.c",
    "src/sign_extending_constant_bitwidth.c",
    "src/sign_extending_variable_bitwidth.c",
};

static ASYNC_COMMAND BuildAndRunAsync(const char *srcPath)
{
    char name[MAX_PATH];
    lstrcpyA(name, PathFindFileName(srcPath));
    PathRemoveExtension(name);

    // Build and run the source files asynchronously.
    return AsyncRunCommand("cl.exe /nologo /Zi /Fo:"BUILD_DIR"\\obj\\%s.obj /Fe:"BUILD_DIR"\\bin\\%s.exe "
	                   "/Fd:"BUILD_DIR"\\bin\\%s.pdb %s && .\\"BUILD_DIR"\\bin\\%s.exe", name, name, name, srcPath, name);
}

int main(int argc, char** argv)
{
    REBUILD_SELF();

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) {
	Fail("Standard output handle is invalid");
	return 1;
    }

    int FileCount = sizeof(SourceFilePaths) / sizeof(*SourceFilePaths);
    ASYNC_COMMAND *Commands = (ASYNC_COMMAND *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ASYNC_COMMAND) * FileCount);

    // Start all tasks asynchronously
    for (int i = 0; i < FileCount; i++)
    {
	Commands[i] = BuildAndRunAsync(SourceFilePaths[i]);
    }

    // Wait for tasks and report results
    for (int i = 0; i < FileCount; i++)
    {
	const char *Path = SourceFilePaths[i];
	int ExitCode = WaitAndReportCommand(Commands[i]);

	// Logging result to the standard output
	Print(hStdOut, "[%d/%d] ", i + 1, FileCount);

	DWORD FOREGROUND_WHITE = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED;
	switch (ExitCode)
	{
	case 0: // Compiled and ran successfully
	    SetConsoleTextAttribute(hStdOut, FOREGROUND_GREEN);
	    Print(hStdOut, "PASS");
	    SetConsoleTextAttribute(hStdOut, FOREGROUND_WHITE);
	break;
	default: // Failed to compile or run
	    SetConsoleTextAttribute(hStdOut, FOREGROUND_RED);
	    Print(hStdOut, "FAIL");
	    SetConsoleTextAttribute(hStdOut, FOREGROUND_WHITE);
	break;
	}

	PrintLine(" %s", Path);
    }

    HeapFree(GetProcessHeap(), 0, Commands);
    return 0;
}
