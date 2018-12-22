/* AP 15-sep-05 a talk program using internet domain sockets */

# include <windows.h>
# include <winsock.h>
# include <io.h>
# include <fcntl.h>
# include <stdio.h>
# include <string.h>

# include "sock.h"

static int line, conn;
HANDLE commThreads[2];

DWORD WINAPI con2sock(void * v)
{
    static char buf[BUFSIZ];
    int rc, len = BUFSIZ - 1;

    if (isatty(fileno(stdin))) len = 1;
    while ((rc = fread(buf, sizeof(char), len, stdin)) > 0)
        send(conn, buf, rc, 0);
    return 1;
}

DWORD WINAPI sock2con(void * v)
{
    static char buf[BUFSIZ];
    int rc, len = BUFSIZ - 1;

    while ((rc = recv(conn, buf, len, 0)) > 0)
        fwrite(buf, sizeof(char), rc, stdout);
    return 1;
}

main(int argc, char ** argv)
{
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(1, 0);
    struct sockaddr * sadr;
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
        fprintf(stderr, "usage: %s <host:port>\n",
                progname);
        exit(1);
    }
    name = *argv++;
    argc--;

    if ((sadr = makeSockAddr(name, TCP)) == NULL) {
        fprintf(stderr, "not a valid address: %s\n", name);
        exit(2);
    }
    conn = socket(AF_INET, SOCK_STREAM, 0);
    if ((connect(conn, sadr, sizeof(struct sockaddr_in))) == SOCKET_ERROR) {
        closesocket(conn);
        name = strchr(name, ':');
        fprintf(stderr, "connect failed, entering server mode on port %s\n", name);
        line = socket(AF_INET, SOCK_STREAM, 0);
        sadr = makeSockAddr(name, TCP);
        if (bind(line, sadr, sizeof(struct sockaddr_in))
            == SOCKET_ERROR) {
            fprintf(stderr, "could not bind address %s\n", name);
            exit(3);
        }
        if (listen(line, 2) == SOCKET_ERROR) {
            fprintf(stderr, "listen failed\n");
            exit(4);
        }
        if ((conn = accept(line, NULL, NULL)) == INVALID_SOCKET) {
            fprintf(stderr, "accept failed\n");
            exit(5);
        }
        closesocket(line);
    }
    /* now we have a connected socket */
    fprintf(stderr, "connected\n");

    commThreads[0] = CreateThread(NULL, 0, sock2con, NULL, 0, NULL);
    commThreads[1] = CreateThread(NULL, 0, con2sock, NULL, 0, NULL);

    WaitForMultipleObjects(2, commThreads, FALSE, INFINITE);

    closesocket(conn);
    WSACleanup();
}
