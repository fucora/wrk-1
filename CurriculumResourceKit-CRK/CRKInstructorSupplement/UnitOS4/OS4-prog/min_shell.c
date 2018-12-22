/* Andreas Polze 08-jan-05				*/
/* min_shell.c						*/
/* Example solution to assignment 4.4.2			*/
/* Windows Operating System Internals			*/

# include <windows.h>
# include <stdio.h>

# define BUF_SIZ 1024

int main(int argc, char* argv[])
{
    char cCommands[BUF_SIZ];
    char * cNewLine;
    char * cCommandline = cCommands;

    /*  read input   */
    if (argc > 2) {
        printf("usage: min_shell {<filename>}\n");
        return -1;
    } else if (argc == 2) {
        HANDLE hFile;
        DWORD dwRead;
        BOOL bResult;

        hFile = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            printf("can't open file, error code %i\n", GetLastError());
            return -2;
        }
        bResult = ReadFile(hFile, &cCommands, sizeof(cCommands), &dwRead, NULL);
        CloseHandle(hFile);
        if (!bResult) {
            printf("can't read file, error code ", GetLastError());
            return -3;
        }
        if (dwRead >= sizeof(cCommands)) {
            printf("not enough buffer space\n");
            return -4;
        }
        cCommands[dwRead] = 0;
    } else {
        if (fgets(cCommands, BUF_SIZ, stdin))
            cCommands[strlen(cCommands) - 1] = 0;
    }

    /* - create processes -  */
    do {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        cNewLine = strchr(cCommandline, '\n');
        if (cNewLine != NULL) {
            *cNewLine = 0;
            *(cNewLine - 1) = 0;
        }
        if (strlen(cCommandline) > 0) {
            ZeroMemory(&si, sizeof(si));

            if (!CreateProcess(NULL, cCommandline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
                printf("process creation failed, code %i\n", GetLastError());
            else if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED) {
                printf("sync object failed, error code %i", GetLastError());
                return -5;
            }

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        cCommandline = cNewLine + 1;
    } while (cNewLine != NULL);

    return 0;
}
