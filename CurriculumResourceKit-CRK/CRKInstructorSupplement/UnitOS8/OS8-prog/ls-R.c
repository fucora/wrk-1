/* AP 30-aug-05	ls-R.c 				*/
/*		list recurseively a directory	*/

/* ls-R.exe <directory>				*/


# include <windows.h>
# include <stdio.h>
# include <stdarg.h>

BOOL TraverseDirectory(LPCTSTR);


int main(int argc, char * argv[])
{
    char * list_dir, curr_dir[MAX_PATH + 1];
    char * progname = argv[0];
    BOOL ok;

    if (argc < 2)
        fprintf(stderr, "usage %s <directory>\n", progname), exit(1);
    list_dir = argv[1];
    printf("listing %s\n", list_dir);

    GetCurrentDirectory(MAX_PATH, curr_dir); /* Save. */
    SetCurrentDirectory(list_dir);

    ok = TraverseDirectory("*");

    SetCurrentDirectory(curr_dir);	 /* Restore working directory. */
    return ok ? 0 : 1;
}


static BOOL TraverseDirectory(LPCTSTR PathName)
{
    HANDLE SearchHandle;
    WIN32_FIND_DATA FindData;
    DWORD pass;
    TCHAR curr_dir[MAX_PATH + 1];

    /* Open up the directory search handle and get the first file name to satisfy the path name.
        Make two passes. The first processes the files and the second processes the directories. */

    GetCurrentDirectory(MAX_PATH, curr_dir);

    for (pass = 1; pass <= 2; pass++) {
        SearchHandle = FindFirstFile(PathName, &FindData);
        if (SearchHandle == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "Error opening Search Handle\n");
            return FALSE;
        }
        /* For each file located, get the type. 	*/
        /* List everything on pass 1.  			*/
        /* On pass 2, display the directory name	*/
        /* and recursively process the subdirectory 	*/
        do {
            if (pass == 1) printf("%s\n", FindData.cFileName);

            /* Traverse the subdirectory on the second pass. */
            if ((pass == 2) && 
                (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && 
                (strcmp(FindData.cFileName, ".")) && 
                (strcmp(FindData.cFileName, ".."))) 
            {
                printf("\n%s\\%s:", curr_dir, FindData.cFileName);
                SetCurrentDirectory(FindData.cFileName);
                TraverseDirectory("*");
                SetCurrentDirectory("..");
            }
        } while (FindNextFile(SearchHandle, &FindData));
        FindClose(SearchHandle);
    }

    return TRUE;
}