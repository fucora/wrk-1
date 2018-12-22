

CC=cl # -DNO_GETDTABLESIZE -D_WIN32 -DVERBOSE
SOCKLIBS=wsock32.lib ws2_32.lib user32.lib gdi32.lib #/NODEFAULTLIB:libc.lib libcmt.lib

LDFLAGS=$(SOCKLIBS)

.c.exe:
	$(CC) $(CFLAGS) $< $(LDFLAGS)
.c.obj:
	$(CC) -c $(CFLAGS) $< 

all:	rcat.exe receive.exe talk.exe echos.exe echoc.exe

RCAT_OBJ =  sock.obj rcat.obj
RCV_OBJ	 =  sock.obj receive.obj
TALK_OBJ =  sock.obj talk.obj
ECHOS_OBJ = sock.obj echos.obj
ECHOC_OBJ = sock.obj echoc.obj


rcat.obj	: sock.h
receive.obj : sock.h
talk.obj : sock.h
echos.obj: sock.h
echoc.obj: sock.h


rcat.exe:	$(RCAT_OBJ)
	$(CC) $(CFLAGS) -o rcat.exe $(RCAT_OBJ) $(LDFLAGS)

receive.exe : $(RCV_OBJ)
	$(CC) $(CFLAGS) -o receive.exe $(RCV_OBJ) $(LDFLAGS)

talk.exe : $(TALK_OBJ)
	$(CC) $(CFLAGS) -o talk.exe $(TALK_OBJ) $(LDFLAGS)

echos.exe : $(ECHOS_OBJ)
	$(CC) $(CFLAGS) -o echos.exe $(ECHOS_OBJ) $(LDFLAGS)

echoc.exe : $(ECHOC_OBJ)
	$(CC) $(CFLAGS) -o echoc.exe $(ECHOC_OBJ) $(LDFLAGS)

clean:	
	del *.obj *.exe
