/* Andreas Polze 08-jan-05                              */
/* mytime.c                                             */
/* Example solution to assignment 4.4.3                 */
/* Windows Operating System Internals                   */

# include <windows.h>
# include <stdio.h>

int main(int argc, char* argv[])
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    SYSTEMTIME ElTiSys, StartTSys, ExitTSys;
    union
    {
        FILETIME ft;
        LONGLONG lt;
    } CreateT, ExitT, ElapsedT;

    if (argc < 1) {
        printf("usage: mytime <command>\n");
        return -1;
    }

    GetSystemTime(&StartTSys);
    ZeroMemory(&si, sizeof(si));
    if (!CreateProcess(NULL, argv[1], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("process creation failed, code %i\n", GetLastError());
        return -2;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetSystemTime(&ExitTSys);

    SystemTimeToFileTime(&StartTSys, &CreateT.ft);
    SystemTimeToFileTime(&ExitTSys, &ExitT.ft);
    /* - 64bit arithmetic on FILETIME - */
    ElapsedT.lt = ExitT.lt - CreateT.lt;
    FileTimeToSystemTime(&ElapsedT.ft, &ElTiSys);

    printf("Real Time: %02d:%02d:%02d:%03d\n", ElTiSys.wHour, ElTiSys.wMinute, ElTiSys.wSecond, ElTiSys.wMilliseconds);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}
