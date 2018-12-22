/* AP 30-aug-05	mymv.c 					*/
/*		move files and directories recursively  */

/* mymv.exe <directory>	<directory>		*/
/* mymv.exe <file>	<directory>		*/
/* mymv.exe <file>	<file>			*/


# include <windows.h>
# include <stdio.h>
# include <stdarg.h>


int main(int argc, char * argv[])
{
    char * src, *dest;
    char * progname = argv[0];
    HANDLE hFile;
    BOOL src_is_dir = FALSE, dest_is_dir = FALSE, dest_exists = FALSE;
    BOOL ok;

    if (argc < 3) {
        fprintf(stderr, "usage %s <directory> <directory>\n", progname);
        fprintf(stderr, "usage %s <file> <directory>\n", progname);
        fprintf(stderr, "usage %s <file> <file>\n", progname);
        exit(1);
    }
    src = argv[1];
    dest = argv[2];

    /* check existence of src file or directory */
    /* with FILE_FLAG_BACKUP_SEMANTICS CreateFile() will open directories*/
    if ((hFile = CreateFile(src, 0, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL)) == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "%s does not exist\n", src);
        exit(2);
    }
    CloseHandle(hFile);
    src_is_dir = (GetFileAttributes(src) & FILE_ATTRIBUTE_DIRECTORY);

    /* check type of destination if already existent */
    if ((hFile = CreateFile(dest, 0, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL)) != INVALID_HANDLE_VALUE) {
        dest_exists = TRUE;
        dest_is_dir = (GetFileAttributes(dest) & FILE_ATTRIBUTE_DIRECTORY);
        CloseHandle(hFile);
    }
    /* cannot move directory onto regular file */
    if (src_is_dir && !dest_is_dir && dest_exists) {
        fprintf(stderr, "%s is not a directory\n", dest);
        exit(3);
    }
    /* create valid path name if destination is directory */
    if (dest_is_dir) {
        /* MoveFileEx() fails if dest is not a file name*/
        /* make up a file name for dest			*/
        char * p = src, *p1;
        if ((p1 = strrchr(p, '\\')) != NULL) p = p1 + 1;
        p1 = malloc(strlen(p) + strlen(dest) + 2);
        sprintf(p1, "%s\\%s", dest, p);
        dest = p1;
    }
    if ((hFile = CreateFile(dest, 0, 0, NULL, OPEN_EXISTING, 0, NULL)) != INVALID_HANDLE_VALUE) {
        fprintf(stderr, "%s already exists\n", dest);
        exit(4);
    }
    printf("moving %s %s\n", src, dest);
    /* for files, MoveFileEx works across volumes */
    if (!src_is_dir) {
        return MoveFileEx(src, dest, MOVEFILE_COPY_ALLOWED);
    }
    /* now src and dest are directories; try MoveFileEx() -	*/
    /* both might be located on the same volume		*/
    ok = MoveFileEx(src, dest, MOVEFILE_COPY_ALLOWED);
    if (ok)
        return 0;

    printf("need to recursively copy directories to other volume\n");
    {
        char curr_dir[MAX_PATH + 1];
        char full_dest[MAX_PATH + 1], *fname;
        GetCurrentDirectory(MAX_PATH, curr_dir); /* Save. */
        GetFullPathName(dest, sizeof(full_dest), full_dest, &fname);
        CreateDirectory(full_dest, NULL);
        SetCurrentDirectory(src);

        ok = RecursivelyCopyDirectory(full_dest);

        SetCurrentDirectory(curr_dir);	 /* Restore working dir */
        RemoveDirectory(src);
    }

    return ok ? 0 : 1;
}


static BOOL RecursivelyCopyDirectory(char * dest)
{
    HANDLE SearchHandle;
    WIN32_FIND_DATA FindData;
    int pass;
    char curr_dir[MAX_PATH + 1], dest_dir[MAX_PATH + 1];
    char dest_fname[MAX_PATH + 1];

    /* Open up the directory search handle and get directory content */
    /* Make three passes. The first processes the directories	*/
    /* and creates a directory hierarchy at destination,	*/
    /* the second moves files, and the third deletes	*/
    /* everything */

    GetCurrentDirectory(MAX_PATH, curr_dir);

    for (pass = 1; pass <= 3; pass++) {
        SearchHandle = FindFirstFile("*", &FindData);
        if (SearchHandle == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "Error opening Search Handle\n");
            return FALSE;
        }
        /* For each file located, get the type. */
        do {
            /* Traverse the subdirectory on the first pass. */
            if ((pass == 1) && 
                (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && 
                (strcmp(FindData.cFileName, ".")) && 
                (strcmp(FindData.cFileName, "..")))
            {
                sprintf(dest_dir, "%s\\%s", dest, FindData.cFileName);
                CreateDirectory(dest_dir, NULL);
                SetCurrentDirectory(FindData.cFileName);
                RecursivelyCopyDirectory(dest_dir);
                SetCurrentDirectory("..");
            }

            if ((pass == 2) && !(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                sprintf(dest_fname, "%s\\%s", dest, FindData.cFileName);
                MoveFileEx(FindData.cFileName, dest_fname, MOVEFILE_COPY_ALLOWED);
            }
            if (pass == 3) {
                /* at this stage only empty subdirectories */
                /* are left over - if any */
                RemoveDirectory(FindData.cFileName);
            }
        } while (FindNextFile(SearchHandle, &FindData));
        FindClose(SearchHandle);
    }

    return TRUE;
}