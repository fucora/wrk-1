
all: talks.exe talkc.exe bakery.exe prodcon.exe prodcon_mutex.exe

bakery.exe: bakery.obj bakery_main.obj
	cl -o bakery.exe bakery.obj bakery_main.obj

bakery.obj: bakery.c bakery.h
bakery_main.obj: bakery_main.c bakery.h
