/* Andreas Polze 08-jan-05              */
/* min_shell_ext.c                  */
/* Example solution to assignment 4.4.4         */
/* Windows Operating System Internals           */


# include <windows.h>
# include <stdio.h>

# define BUF_SIZ    1024
# define NPROC_ASYNC    128


int main(int argc, char* argv[])
{
    HANDLE hProcAsync[NPROC_ASYNC];
    char cCommands[BUF_SIZ];
    char cCurDir[BUF_SIZ];
    int iProcPtr = 0;
    char * cCommandline = cCommands;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    do {
        /* - write command prompt -- */
        GetCurrentDirectory(sizeof(cCurDir), cCurDir);
        printf("%s>", cCurDir);
        if (fgets(cCommands, BUF_SIZ - 1, stdin))
            cCommands[strlen(cCommands) - 1] = 0;

        if (strlen(cCommandline) > 0) {
            if (!strcmp(cCommandline, "exit")) {
                /* - process exit command  */
                return 0;
            } else if (!strncmp(cCommands, "cd ", 3)) {
                /* - process cd command - */
                if (!SetCurrentDirectory(&cCommands[3]))
                    printf("invalid Directory\n");
            } else if (!strcmp(cCommandline, "wait")) {
                /* - process wait command  */
                /* - wait for all async processes -- */
                if (!iProcPtr)
                    printf("nothing to wait for\n");
                else if (WaitForMultipleObjects(iProcPtr, hProcAsync, TRUE, INFINITE) == WAIT_FAILED)
                    printf("wait failed, error code %i", GetLastError());
                while (iProcPtr)
                    CloseHandle(hProcAsync[iProcPtr--]);
            } else {
                /* - check for ampersand  */
                char* cAmpersand = strrchr(cCommandline, '&');
                if (cAmpersand) *cAmpersand = 0;

                ZeroMemory(&si, sizeof(si));
                if (!CreateProcess(NULL, cCommandline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                    printf("process creation failed, error code %i\n", GetLastError());
                } else {
                    /*  wait for synchronously executed process -- */
                    if (!cAmpersand) {
                        if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED)
                            printf("wait failed, error code %i", GetLastError());
                        CloseHandle(pi.hProcess);
                    } else {
                        /*  register asynchronously started process -- */
                        if (iProcPtr > sizeof(hProcAsync))
                            printf("not enough buffer space\n");
                        else
                            hProcAsync[iProcPtr++] = pi.hProcess;
                    }
                    CloseHandle(pi.hThread);
                }
            }
        }
    } while (1);
}