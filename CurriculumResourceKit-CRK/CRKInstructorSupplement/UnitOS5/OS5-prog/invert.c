/* AP 10-mar-05 invert.c - producer/consumer app using multiple heaps   */
/* Example solution to assignment 5.2.4                                 */
/* Windows Operating System Internals                                   */

# include <windows.h>
# include <stdio.h>

# define RESERVE_LIMIT 	(1024*1024*1024)	// 1 GByte section
# define COMMIT_CHUNK 	(16*1024)		// 16 KByte blocks

static SIZE_T commited;
static char * start, *curr, *last;

main()
{
    unsigned int cnt;
    int finished;

    start = VirtualAlloc(NULL, 	       	// region to reserve or commit
                         RESERVE_LIMIT,   	// size of region
                         MEM_RESERVE,  	// type of allocation
                         PAGE_READWRITE); // type of access protection
    if (!start) fprintf(stderr, "Reserving 1GB failed\n"), exit(1);
    last = curr = start;

    finished = feof(stdin);
    while (!finished) {
        if (VirtualAlloc(curr, COMMIT_CHUNK,
                         MEM_COMMIT, PAGE_READWRITE) != curr)
            fprintf(stderr, "Commiting 1MB failed\n"), exit(2);
        last += COMMIT_CHUNK;
        do {
            cnt = fread(curr, sizeof(char), last - curr, stdin);
            curr += cnt;
            finished = feof(stdin);
        } while ((curr < last) && !finished);
    }
    last = curr;

    /* detect nl and print text in inverse ordering */
    while (curr != start) {
        if (*curr == '\n') {
            cnt = last - curr - 1;
            if (fwrite(curr + 1, sizeof(char), cnt, stdout) != cnt)
                fprintf(stderr, "fwrite failed\n"), exit(3);
            last = curr + 1;
        }
        curr--;
    }
    cnt = last - curr;
    if (fwrite(curr, sizeof(char), cnt, stdout) != cnt)
        fprintf(stderr, "fwrite failed\n"), exit(4);
}
