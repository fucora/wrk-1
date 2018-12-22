/* AP 15-sep-05 sock.c	routines for dealing with sockets */

# include <winsock.h>
# include <stdio.h>
# include <string.h>
# include <memory.h>
# include "sock.h"

struct sockaddr * makeSockAddr(char * name, char * mode)
{
    int r;
    static struct sockaddr_in sadr;
    struct hostent * hp;
    struct servent * sp;
    char * cp;
    short port;

    //unsigned long inet_addr();
    if (!(cp = strchr(name, ':'))) {
        fprintf(stderr, "%s : invalid adress format\n", name);
        return NULL;
    }   /* not host:port format */
    memset((char*)& sadr, '\0', sizeof sadr);
    sadr.sin_family = AF_INET;
    *cp = '\0';

    if (cp == name)
        sadr.sin_addr.s_addr = htonl(INADDR_ANY);
    else if (strlen(name) == strspn(name, ".0123456789"))
        sadr.sin_addr.s_addr = inet_addr(name);
    else if (hp = gethostbyname((char *)name))
        memcpy((char *)& sadr.sin_addr, (char *)hp->h_addr, hp->h_length);
    else {
        fprintf(stderr, "%s : invalid hostname\n", name);
        *cp = ':'; return 0;
    }
    *cp++ = ':';

    if (sscanf(cp, "%hd", &port) == 1)
        sadr.sin_port = htons(port);
    else {
        sp = getservbyname(cp, mode);
        if (!sp) {
            fprintf(stderr, "%s : port or service must be specified\n", name);
            return NULL;
        }
        sadr.sin_port = sp->s_port;
    }

    return (struct sockaddr *) &sadr;
}
