/* AP 02-sep-05	ac2uc.c					*/
/*		convert an ascii file to unicode	*/
/*		and vice versa depending on name	*/
/*		of the executable			*/

/* ac2uc.exe [-s][-b <buf size>] <file> <file>		*/
/* uc2ac.exe [-s][-b <buf size>] <file> <file>		*/

# include <windows.h>
# include <stdio.h>
# include <stdarg.h>

static int BUF_SIZE = 256;

int main(int argc, char ** argv)
{
    char * progname = *argv++;
    char * src, *dest;
    CHAR *aBuf;
    WCHAR *uBuf;
    int i, rcnt, wcnt;
    HANDLE hIn, hOut;
    BOOL convert_ac2uc;
    DWORD flags_and_attrs = FILE_ATTRIBUTE_NORMAL;

    while ((argc > 1) && (*argv)[0] == '-') {
        if (strcmp(*argv, "-s") == 0) {
            flags_and_attrs |= FILE_FLAG_SEQUENTIAL_SCAN;
            argv++; argc--;
        }
        if (strcmp(*argv, "-b") == 0) {
            BUF_SIZE = atoi(*(++argv));
            if (BUF_SIZE <= 0)
                fprintf(stderr, "invalid buffer size %d\n", BUF_SIZE), exit(1);

            argv++; argc--;
        }
    }

    if (argc < 3)
        fprintf(stderr, "usage: %s [-s][-b <buf size>] <file> <file>\n", progname), exit(1);
    if (strcmp(progname, "ac2uc") == 0)
        convert_ac2uc = TRUE;
    else if (strcmp(progname, "uc2ac") == 0)
        convert_ac2uc = FALSE;
    else
        fprintf(stderr, "%s should be ac2uc.exe or uc2ac.exe\n", progname), exit(2);

    src = *argv++;
    dest = *argv;

    aBuf = VirtualAlloc(NULL, BUF_SIZE * sizeof(CHAR), MEM_COMMIT, PAGE_READWRITE);
    uBuf = VirtualAlloc(NULL, BUF_SIZE * sizeof(WCHAR), MEM_COMMIT, PAGE_READWRITE);
    if ((aBuf == NULL) || (uBuf == NULL))
        fprintf(stderr, "could not allocate buffers\n"), exit(3);

    /* open src file */
    if ((hIn = CreateFile(src, GENERIC_READ, 0, NULL, OPEN_EXISTING, flags_and_attrs, NULL)) == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "%s does not exist\n", src);
        exit(4);
    }

    /* open dest file */
    if ((hOut = CreateFile(dest, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, flags_and_attrs, NULL)) == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "could not create ouput file %s\n", dest);
        exit(5);
    }

    if (convert_ac2uc) {
        do {
            ReadFile(hIn, aBuf, sizeof(aBuf), &rcnt, NULL);
            for (i = 0; i < rcnt; i++)
                uBuf[i] = (WCHAR)aBuf[i];
        } while ((rcnt > 0) && WriteFile(hOut, uBuf, 2 * rcnt, &wcnt, NULL));
    } else {
        do {
            ReadFile(hIn, uBuf, 2 * sizeof(uBuf), &rcnt, NULL);
            for (i = 0; i < rcnt; i++)
                aBuf[i] = (CHAR)uBuf[i];
        } while ((rcnt > 0) && WriteFile(hOut, aBuf, rcnt / 2, &wcnt, NULL));
    }
    CloseHandle(hIn);
    CloseHandle(hOut);
}
