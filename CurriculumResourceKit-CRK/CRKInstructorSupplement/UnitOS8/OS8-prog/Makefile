
all: ls-R.exe mymv.exe ac2uc.exe uc2ac.exe

clean:
	del *.exe *.obj

uc2ac.exe: ac2uc.exe
	del uc2ac.exe
	ln ac2uc.exe uc2ac.exe
#	copy ac2uc.exe uc2ac.exe # use this if you have no ln

