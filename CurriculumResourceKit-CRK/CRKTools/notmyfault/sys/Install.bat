copy %BASEDIR%\lib\%CPU%\%DDKBUILDENV%\myfault.sys .

copy myfault.sys ..\exe\release\.
copy myfault.sys ..\exe\debug\.
rmdir /q /s obj 
del *.log


