/* AP 	18-apr-05 bakery.c			*/
/*	an implementation of critical		*/
/*	sections based on bakery algorithm	*/

# include "bakery.h"

# define MAX_THREAD	1024

struct _crit_sec
{
    volatile int choosing[MAX_THREAD];
    volatile int number[MAX_THREAD];
};


void My_InitializeCriticalSection(MY_LPCRITICAL_SECTION * sec)
{
    int i;
    *sec = (MY_LPCRITICAL_SECTION)malloc(sizeof(struct _crit_sec));
    for (i = 0; i < MAX_THREAD; i++) {
        (*sec)->choosing[i] = 0;
        (*sec)->number[i] = 0;
    }
}


void My_DeleteCriticalSection(MY_LPCRITICAL_SECTION sec)
{
    if (sec) free(sec);
}


void My_EnterCriticalSection(int myId, MY_LPCRITICAL_SECTION sec)
{
    int max, j;
    sec->choosing[myId] = 1;
    max = 0;
    for (j = 0; j < MAX_THREAD; j++) {
        if (max < sec->number[j]) max = sec->number[j];
    }
    sec->number[myId] = max + 1;
    sec->choosing[myId] = 0;
    for (j = 0; j < MAX_THREAD; j++) {
        while (sec->choosing[j]);
        while (sec->number[j] != 0 && (sec->number[j] < sec->number[myId] || (sec->number[j] == sec->number[myId] && j < myId)));
    }
}


void My_LeaveCriticalSection(int myId, MY_LPCRITICAL_SECTION sec)
{
    sec->number[myId] = 0;
}
