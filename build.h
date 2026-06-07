#if !defined(_BUILD_H)
#define _BUILD_H

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>

void Print(HANDLE hOut, const char* msg);
void PrintLine(const char* msg);
void Fail(const char* msg);

int RunCommand(const char* cmd);

void RebuildSelf(const char* sourcePath);
#define REBUILD_SELF() RebuildSelf(__FILE__)

#endif // _BUILD_H

#if defined(BUILD_IMPLEMENTATION)

void Print(HANDLE hOut, const char* msg)
{
    if (!msg) return;
    DWORD written;
    WriteFile(hOut, msg, (DWORD)lstrlenA(msg), &written, NULL);
}

void PrintLine(const char* msg)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    Print(hOut, msg);
    Print(hOut, "\r\n");
}

void Fail(const char* msg)
{
    HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
    Print(hErr, "FATAL ERROR: ");
    Print(hErr, msg);
    Print(hErr, "\r\n");
    ExitProcess(1);
}

int RunCommand(const char* cmd)
{
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    // Copy command into buffer wrapped in cmd.exe
    const char cmdPrefix[]  = "cmd.exe /c \"";
    const char cmdPostfix[] = "\"";

    char cmdBuffer[8192];
    int cmdLength = lstrlenA(cmd);
    if ((cmdLength + sizeof(cmdPrefix) + sizeof(cmdPostfix) - 1) >= sizeof(cmdBuffer)) {
        Fail("Command line too long for RunCommand buffer");
    }
    lstrcpyA(cmdBuffer, cmdPrefix);
    lstrcpyA(cmdBuffer + sizeof(cmdPrefix) - 1, cmd);
    lstrcpyA(cmdBuffer + sizeof(cmdPrefix) - 1 + cmdLength, cmdPostfix);

    if (!CreateProcessA(NULL, cmdBuffer, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        return GetLastError() ? GetLastError() : 1;
    }
    WaitForSingleObject(pi.hProcess, INFINITE); // Synchronous execution

    DWORD exitCode = 1;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (int)exitCode;
}

/* Checks if file a is newer than file b */
static int IsFileNewer(const char* a, const char* b)
{
    WIN32_FILE_ATTRIBUTE_DATA fa;
    WIN32_FILE_ATTRIBUTE_DATA fb;

    if (!GetFileAttributesExA(a, GetFileExInfoStandard, &fa))
    {
        return 0; // Source file not available
    }

    if (!GetFileAttributesExA(b, GetFileExInfoStandard, &fb))
    {
        return 1; // Executable file not available
    }

    return CompareFileTime(&fa.ftLastWriteTime, &fb.ftLastWriteTime) > 0;
}

void RebuildSelf(const char* sourcePath)
{
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    if (!IsFileNewer(sourcePath, exePath)) return;

    HANDLE hMutex = CreateMutexA(NULL, FALSE, "Local\\SelfRebuildingBuildExeMutex");
    if (hMutex)
    {
        WaitForSingleObject(hMutex, INFINITE);
    }

    if (!IsFileNewer(sourcePath, exePath))
    {
        if (hMutex)
        { 
            ReleaseMutex(hMutex);
            CloseHandle(hMutex);
        }
        return; 
    }

    PrintLine("Rebuilding self...");

    // Postfix the running executable's name with '.old'
    char oldPath[MAX_PATH];
    lstrcpyA(oldPath, exePath);
    lstrcatA(oldPath, ".old");

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
    char cmd[1024];
    wsprintfA(cmd, "cl.exe /nologo %s /Fe:%s /link user32.lib > NUL", sourcePath, exePath);
    if (RunCommand(cmd) != 0)
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
    char* original_cmd = GetCommandLineA();
    int newRunExitCode = RunCommand(original_cmd);

    if (hMutex)
    {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }

    ExitProcess(newRunExitCode);
}

#endif // BUILD_IMPLEMENTATION
