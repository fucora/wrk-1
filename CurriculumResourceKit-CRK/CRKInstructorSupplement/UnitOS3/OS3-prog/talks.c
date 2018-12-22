/* AP 18-apr-05	talks.c						*/
/*		simple talk server program using named pipes	*/

# include <windows.h>
# include <stdio.h>

# define BUF_SIZ	256
# define CS_TIMEOUT 	5000
# define QUIT		"exit\n"

main(int argc, char * argv[])
{
    char * progname = argv[0];
    char pipename[100];
    char inb[BUF_SIZ], outb[BUF_SIZ];
    int nXfer, Done = 0;

    HANDLE hNamedPipe;
    LPSECURITY_ATTRIBUTES pNPSA = NULL;

    if (argc < 2) {
        fprintf(stderr, "usage: %s <pipe_name>\n", progname);
        exit(1);
    }
    sprintf(pipename, "\\\\.\\pipe\\%s", argv[1]);
    hNamedPipe = CreateNamedPipe(pipename, 
                                 PIPE_ACCESS_DUPLEX,
                                 PIPE_READMODE_MESSAGE | PIPE_TYPE_MESSAGE | PIPE_WAIT,
                                 1,
                                 0,
                                 0, 
                                 CS_TIMEOUT,
                                 pNPSA);

    while (!Done) {		/* Main Command Loop. */
        printf("\t\t\tServer is awaiting next request.\n");
        if (!ConnectNamedPipe(hNamedPipe, NULL) || !ReadFile(hNamedPipe, &inb, BUF_SIZ, &nXfer, NULL)) {
            fprintf(stderr, "Connect or Read Named Pipe error.");
            exit(2);
        }
        printf(inb); fflush(stdout);
        if (strcmp(inb, QUIT) == 0) {
            Done = 1;
            printf("\t\t\tServer is quitting now.\n");
            strcpy(outb, "");
        } else {
            fgets(outb, BUF_SIZ, stdin);
        }
        WriteFile(hNamedPipe, &outb, (strlen(outb) + 1), &nXfer, NULL);
        DisconnectNamedPipe(hNamedPipe);
    }
}
