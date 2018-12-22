/* AP 15-sep-05 remote cat using internet domain sockets */

# include <windows.h>
# include <winsock.h>
# include <io.h>
# include <fcntl.h>
# include <stdio.h>
# include <string.h>

# include "sock.h"

static int doCat(int sock, char * fname)
{
    static char buf[BUFSIZ];
    int rc, len = BUFSIZ - 1;
    FILE * f;

    if (strcmp(fname, "-") == 0) f = stdin;
    else f = fopen(fname, "r");
    if (!f) return 0;
    if (isatty(fileno(f))) len = 1;

    while (rc = fread(buf, sizeof(char), len, f))
        send(sock, buf, rc, 0);
    if (f != stdin) fclose(f);
    return 1;
}

main(int argc, char ** argv)
{
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(1, 0);
    struct sockaddr * sadr;
    int sock;
    char * progname = *argv++;
    char * name;

    argc--;
    if (WSAStartup(wVersionRequested, &wsaData)) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        fprintf(stderr, "Could not find Winsock library\n");
        WSACleanup();
        exit(1);
    }
    if (argc < 1) {
        fprintf(stderr, "usage: %s <host:port> [<filename> *]\n", progname);
        exit(1);
    }
    name = *argv++;
    argc--;

    if ((sadr = makeSockAddr(name, TCP)) == NULL) {
        fprintf(stderr, "not a valid address: %s\n", name);
        exit(3);
    }
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if ((connect(sock, sadr, sizeof(struct sockaddr_in))) == SOCKET_ERROR) {
        fprintf(stderr, "connect failed for: %s\n", name);
        exit(4);
    }
    if (argc) {
        do
            doCat(sock, *argv);
        while (++argv, --argc);
    } else {
        doCat(sock, "-");
    }
    closesocket(sock);
    WSACleanup();
}
