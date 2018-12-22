/* AP 15-sep-05  echo client using UDP on internet domain sockets */

# include <winsock.h>
# include <io.h>
# include <fcntl.h>
# include <stdio.h>
# include <string.h>

# include "sock.h"

main(int argc, char ** argv)
{
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(1, 0);
    struct sockaddr * sadr;
    static char buf[BUFSIZ];
    int sock, rcount;
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

    if ((sadr = makeSockAddr(name, UDP)) == NULL) {
        fprintf(stderr, "not a valid address: %s\n", name);
        exit(3);
    }
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    fprintf(stderr, "%s: waiting for input\n", progname);
    do {
        fgets(buf, BUFSIZ, stdin);
        rcount = strlen(buf);
        sendto(sock, buf, rcount, 0, sadr, sizeof(struct sockaddr_in));
        if ((rcount = recvfrom(sock, buf, BUFSIZ - 1, 0, 0, 0)) > 0) {
            fwrite(buf, sizeof(char), rcount, stdout);
        }
    } while (rcount > 0);
    fprintf(stderr, "no server found at %s\n", name);

    WSACleanup();
}
