/* AP 15-sep-05 sock.h    routines for dealing with sockets */

# ifndef __sock_h
# define __sock_h

# define TCP    "tcp"
# define UDP    "udp"

extern struct sockaddr * makeSockAddr( char * name, char * mode );

# endif