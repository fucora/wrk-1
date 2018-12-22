/* AP 18-apr-05	talkc.c						*/
/*		simple talk client program using named pipes	*/

# include <windows.h>
# include <stdio.h>

# define BUF_SIZ	256
# define CS_TIMEOUT 	5000
# define QUIT		"exit"

main(int argc, char * argv[])
{
    char * progname = argv[0];
    char pipename[100];
    char inb[BUF_SIZ], outb[BUF_SIZ];
    int nXfer, Done = 0;

    HANDLE hNamedPipe;
    LPSECURITY_ATTRIBUTES pNPSA = NULL;

    if (argc < 2) {
        fprintf(stderr, "usage: %s <pipe_name> [host]\n", progname);
        exit(1);
    }

    if (argc < 3) {
        sprintf(pipename, "\\\\.\\pipe\\%s", argv[1]);
    } else {
        sprintf(pipename, "\\\\%s\\pipe\\%s", argv[2], argv[1]);
    }

    WaitNamedPipe(pipename, NMPWAIT_WAIT_FOREVER);

    while (1) {		/* Main Command Loop. */
        hNamedPipe = CreateFile(pipename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hNamedPipe == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "Failure to locate server %s\n", pipename);
            exit(1);
        }
        printf("\t\t\tTalking to %s\n", pipename);
        fgets(outb, BUF_SIZ, stdin);
        WriteFile(hNamedPipe, &outb, (strlen(outb) + 1), &nXfer, NULL);
        if (!ReadFile(hNamedPipe, &inb, BUF_SIZ, &nXfer, NULL)) {
            fprintf(stderr, "Read Named Pipe error.");
            exit(2);
        }
        printf(inb); fflush(stdout);
        CloseHandle(hNamedPipe);
    }
}
