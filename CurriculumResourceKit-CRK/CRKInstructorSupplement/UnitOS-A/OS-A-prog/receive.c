/* AP 15-sep-05 receive data via internet domain sockets */

# include <winsock.h>
# include <io.h>
# include <fcntl.h>
# include <stdio.h>
# include <string.h>

# include "sock.h"

# define BUF_LEN 80
# define QUEUE_LEN 1

main(int argc, char ** argv)
{
    static char buf[BUF_LEN];
    char * progname = *argv++;
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(1, 0);
    struct sockaddr * sadr;
    char * name;
    int line, conn;
    int rcount;

    if (WSAStartup(wVersionRequested, &wsaData)) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        fprintf(stderr, "Could not find Winsock library\n");
        WSACleanup();
        exit(1);
    }
    if (argc < 2) {
        fprintf(stderr, "usage: %s :port \n", progname);
        exit(1);
    }
    name = *argv;
    if ((sadr = makeSockAddr(name, TCP)) == NULL) {
        fprintf(stderr, "not a valid port: %s\n", name);
        exit(2);
    }
    line = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(line, sadr, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
        fprintf(stderr, "could not bind address %s\n", name);
        exit(3);
    }
    if (listen(line, QUEUE_LEN) == SOCKET_ERROR) {
        fprintf(stderr, "listen failed\n");
        exit(4);
    }
    if ((conn = accept(line, NULL, NULL)) == INVALID_SOCKET) {
        fprintf(stderr, "accept failed\n");
        exit(5);
    }
    closesocket(line);

    do {
        rcount = recv(conn, buf, BUF_LEN - 1, 0);
        write(1, buf, rcount);
    } while (rcount > 0);

    closesocket(conn);
    WSACleanup();
}
