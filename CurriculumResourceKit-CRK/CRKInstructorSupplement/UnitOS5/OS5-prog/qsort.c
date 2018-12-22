/* AP/BR 10-mar-05 qsort.c              */
/* Example solution to assignment 5.2.2 */
/* Windows Operating System Internals   */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int compare(const void *arg1, const void *arg2);

int main(int argc, char *argv[])
{
    HANDLE hMap, hFile;
    DWORD err, size;
    int elems;
    LPVOID pData;

    if (argc < 2) printf("usage: %s file_to_sort\n", argv[0]), exit(1);

    /* open file for reading and writting */
    hFile = CreateFile(argv[1], GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        fprintf(stderr, "error opening input\n"), exit(2);

    /* determine the size of the file */
    size = GetFileSize(hFile, NULL);
    if (GetLastError() != NO_ERROR)
        fprintf(stderr, "could not get file size\n"), exit(3);

    /* check for valid number of integers */
    if (size % sizeof(int))
        fprintf(stderr, "invalid number of input bytes\n"), exit(4);

    /* calculate the amount of numbers to sort */
    elems = size / sizeof(int);

    /* create the file mapping object */
    if (!(hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL)))
        fprintf(stderr, "CreateFileMapping failed\n"), exit(5);

    /* map the whole file into the process context */
    if (!(pData = MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0)))
        fprintf(stderr, "MapViewOfFile failed\n"), exit(6);

    qsort(pData, elems, sizeof(int), compare);

    CloseHandle(hMap);
    CloseHandle(hFile);

    return 0;
}

/* compare 2 integers */
int compare(const void *arg1, const void *arg2)
{
    if (*((int*)arg1) == *((int*)arg2))
        return 0; /*equal*/
    else if (*((int*)arg1) > *((int*)arg2))
        return 1;
    else return -1;
}
