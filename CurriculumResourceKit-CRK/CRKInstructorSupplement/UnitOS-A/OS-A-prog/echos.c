/* AP 15-sep-05  echo server using UDP on internet domain sockets */

# include <winsock.h>
# include <io.h>
# include <fcntl.h>
# include <stdio.h>
# include <string.h>

# include "sock.h"

# define BUF_LEN 256

main(int argc, char ** argv)
{
    static char buf[BUF_LEN];
    char * progname = *argv++;
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(1, 0);
    struct sockaddr * sadr;
    struct sockaddr_in from_addr;
    int fromlen = sizeof(from_addr);
    int sock, rcount, i;
    char * name;

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
    if ((sadr = makeSockAddr(name, UDP)) == NULL) {
        fprintf(stderr, "not a valid port: %s\n", name);
        exit(2);
    }
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bind(sock, sadr, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
        fprintf(stderr, "could not bind address %s\n", name);
        exit(3);
    }
    fprintf(stderr, "%s: starting server loop\n", progname);
    do {
        rcount = recvfrom(sock, buf, BUF_LEN - 1, 0, (struct sockaddr*) &from_addr, &fromlen);
        for (i = 0; i < rcount; i++)
            buf[i] = toupper(buf[i]);
        sendto(sock, buf, rcount, 0, (struct sockaddr*) &from_addr, fromlen);
    } while (rcount > 0);
    fprintf(stderr, "%s: exiting\n", progname);

    WSACleanup();
}
