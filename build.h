#if !defined(_BUILD_H)
#define _BUILD_H

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef BUILD_DIR
#define BUILD_DIR ".build"
#endif

void Print(HANDLE hOut, const char* fmt, ...);
void PrintLine(const char* fmt, ...);
void Fail(const char* fmt, ...);

typedef struct {
    HANDLE hProcess;
    char tempFile[MAX_PATH];
} ASYNC_COMMAND;

int RunCommand(const char* fmt, ...);
ASYNC_COMMAND AsyncRunCommand(const char* fmt, ...);
int WaitAndReportCommand(ASYNC_COMMAND cmd);

const char* PathFindFileName(const char* path);
void PathRemoveExtension(char* path);

void RebuildSelf(const char* srcPath);
#define REBUILD_SELF() RebuildSelf(__FILE__)

#endif // _BUILD_H

#if defined(BUILD_IMPLEMENTATION)

static char g_InternalBuffer[8192];

const char* PathFindFileName(const char* path)
{
    const char* fileName = path;
    const char* p = path;
    while (*p)
    {
        if (*p == '\\' || *p == '/') fileName = p + 1;
        p++;
    }
    return fileName;
}

void PathRemoveExtension(char* path)
{
    char* lastDot = NULL;
    char* p = path;
    while (*p)
    {
        if (*p == '.') lastDot = p;
        if (*p == '\\' || *p == '/') lastDot = NULL;
        p++;
    }
    if (lastDot) *lastDot = '\0';
}

void Print(HANDLE hOut, const char* fmt, ...)
{
    if (!fmt) return;
    va_list args;
    va_start(args, fmt);
    wvsprintfA(g_InternalBuffer, fmt, args);
    va_end(args);

    DWORD written;
    WriteFile(hOut, g_InternalBuffer, (DWORD)lstrlenA(g_InternalBuffer), &written, NULL);
}

void PrintLine(const char* fmt, ...)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        wvsprintfA(g_InternalBuffer, fmt, args);
        va_end(args);
        Print(hOut, g_InternalBuffer);
    }
    Print(hOut, "\r\n");
}

void Fail(const char* fmt, ...)
{
    HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
    Print(hErr, "FATAL ERROR: ");
    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        wvsprintfA(g_InternalBuffer, fmt, args);
        va_end(args);
        Print(hErr, g_InternalBuffer);
    }
    Print(hErr, "\r\n");
    ExitProcess(1);
}

static ASYNC_COMMAND Internal_VAsyncRunCommand(const char* fmt, va_list args)
{
    ASYNC_COMMAND cmd = {0};

    // Use g_InternalBuffer to construct the command line.
    // We wrap the command in cmd.exe /c "..."
    lstrcpyA(g_InternalBuffer, "cmd.exe /c \"");
    int prefixLen = lstrlenA(g_InternalBuffer);

    wvsprintfA(g_InternalBuffer + prefixLen, fmt, args);
    lstrcatA(g_InternalBuffer, "\"");

    // Setup redirection to a temporary file
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    GetTempFileNameA(tempPath, "bld", 0, cmd.tempFile);

    SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
    HANDLE hFile = CreateFileA(cmd.tempFile, GENERIC_WRITE, FILE_SHARE_READ, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    STARTUPINFOA si = {0};
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdOutput = hFile;
    si.hStdError = hFile;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION pi = {0};

    if (!CreateProcessA(NULL, g_InternalBuffer, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) 
    {
        CloseHandle(hFile);
        DeleteFileA(cmd.tempFile);
        cmd.hProcess = NULL;
	return cmd;
    }

    cmd.hProcess = pi.hProcess;
    CloseHandle(pi.hThread);
    CloseHandle(hFile);

    return cmd;
}

ASYNC_COMMAND AsyncRunCommand(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ASYNC_COMMAND cmd = Internal_VAsyncRunCommand(fmt, args);
    va_end(args);
    return cmd;
}

int WaitAndReportCommand(ASYNC_COMMAND cmd)
{
    if (cmd.hProcess == NULL) return 1;

    WaitForSingleObject(cmd.hProcess, INFINITE);

    DWORD exitCode = 1;
    GetExitCodeProcess(cmd.hProcess, &exitCode);
    CloseHandle(cmd.hProcess);

    if (exitCode != 0)
    {
        // On failure, read and print the output from the temporary file.
        HANDLE hRead = CreateFileA(cmd.tempFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hRead != INVALID_HANDLE_VALUE)
        {
            DWORD bytesRead;
            while (ReadFile(hRead, g_InternalBuffer, sizeof(g_InternalBuffer), &bytesRead, NULL) && bytesRead > 0)
            {
                DWORD written;
                WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), g_InternalBuffer, bytesRead, &written, NULL);
            }
            CloseHandle(hRead);
        }
    }

    DeleteFileA(cmd.tempFile);
    return (int)exitCode;
}

int RunCommand(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ASYNC_COMMAND cmd = Internal_VAsyncRunCommand(fmt, args);
    va_end(args);

    return WaitAndReportCommand(cmd);
}

static int IsFileNewer(const char* srcPath, const char* exePath)
{
    WIN32_FILE_ATTRIBUTE_DATA srcInfo;
    WIN32_FILE_ATTRIBUTE_DATA exeInfo;

    if (!GetFileAttributesExA(srcPath, GetFileExInfoStandard, &srcInfo))
    {
	return 0; // The source does not exist. Cannot rebuild.
    }

    if (!GetFileAttributesExA(exePath, GetFileExInfoStandard, &exeInfo))
    {
	return 1; // The executable does not exist. Rebuild.
    }

    return CompareFileTime(&srcInfo.ftLastWriteTime, &exeInfo.ftLastWriteTime) > 0;
}

static void CreateNewDirectoryUnlessItExists(const char *path)
{
    if (CreateDirectoryA(path, NULL) == 0)
    {
        if (GetLastError() != ERROR_ALREADY_EXISTS)
        {
            Fail("The path `%s` could not be created", path);
        }
    }
}

static void EnsureBuildDirectoriesExist(void)
{
    CreateNewDirectoryUnlessItExists(BUILD_DIR);
    CreateNewDirectoryUnlessItExists(BUILD_DIR "\\bin");
    CreateNewDirectoryUnlessItExists(BUILD_DIR "\\obj");
}

void RebuildSelf(const char* srcPath)
{
    EnsureBuildDirectoriesExist();

    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    if (IsFileNewer(srcPath, exePath) == 0) return;

    HANDLE hMutex = CreateMutexA(NULL, FALSE, "Local\\SelfRebuildingBuildExeMutex");
    if (hMutex)
    {
	WaitForSingleObject(hMutex, INFINITE);
    }

    if (IsFileNewer(srcPath, exePath) == 0)
    {
	if (hMutex)
	{ 
	    ReleaseMutex(hMutex);
	    CloseHandle(hMutex);
	}
	return; 
    }

    PrintLine("Rebuilding self...");

    // Postfix the running executable's name with '.old' and move to BUILD_DIR
    const char *exeName = PathFindFileName(exePath);

    char oldPath[MAX_PATH];
    wsprintfA(oldPath, BUILD_DIR "\\bin\\%s.old", exeName);

    DeleteFileA(oldPath);
    if (!MoveFileExA(exePath, oldPath, MOVEFILE_REPLACE_EXISTING))
    {
	if (hMutex)
	{
	    ReleaseMutex(hMutex);
	    CloseHandle(hMutex);
	}
	Fail("Could not rename current executable");
    }

    // Rebuild the executable.
    char objPath[MAX_PATH], pdbPath[MAX_PATH];
    wsprintfA(objPath, BUILD_DIR "\\obj\\build.obj");
    wsprintfA(pdbPath, BUILD_DIR "\\bin\\build.pdb");

    if (RunCommand("cl.exe /nologo /Zi /Fo:%s /Fd:%s /Fe:%s %s /link user32.lib", 
                   objPath, pdbPath, exePath, srcPath) != 0)
    {
	MoveFileExA(oldPath, exePath, MOVEFILE_REPLACE_EXISTING);
	if (hMutex)
	{
	    ReleaseMutex(hMutex);
	    CloseHandle(hMutex);
	}
	Fail("Source file compilation failed");
    }

    PrintLine("Launching fresh executable...");

    // Run the original command line with the new executable.
    // We do NOT use RunCommand here because we don't want to redirect output to a temp file.
    // We want the fresh executable to print directly to the console.
    // We also do not wait for the process to finish, we just exit.
    char* original_cmd = GetCommandLineA();
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };
    if (!CreateProcessA(NULL, original_cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
        Fail("Could not launch fresh executable");
    }

    if (hMutex)
    {
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
    }

    ExitProcess(0);
}

#endif // BUILD_IMPLEMENTATION
