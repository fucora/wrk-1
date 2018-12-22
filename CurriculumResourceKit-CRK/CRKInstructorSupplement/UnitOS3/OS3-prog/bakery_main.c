/* AP 18-apr-05 three threads synchronizing using bakery algorithm	*/

# include <stdio.h>
# include <windows.h>
# include "bakery.h"

static MY_LPCRITICAL_SECTION lpsec;
static int counter;

DWORD WINAPI thread_func(LPVOID arg)
{
    int myId = (int)arg;
    int iter = 0;

    do {
        My_EnterCriticalSection(myId, lpsec);
        if (myId % 2)
            counter++;
        else
            counter--;

        printf("Thread %d in critical section; counter = %d\n", myId, counter);
        My_LeaveCriticalSection(myId, lpsec);
        iter++;
    } while (iter < 1000);

    return 0;
}


main()
{
    HANDLE th1, th2, th3;
    My_InitializeCriticalSection(&lpsec);

    th1 = CreateThread(NULL, 0, thread_func, (void*)1, 0, NULL);
    th2 = CreateThread(NULL, 0, thread_func, (void*)2, 0, NULL);
    th3 = CreateThread(NULL, 0, thread_func, (void*)3, 0, NULL);

    WaitForSingleObject(th1, INFINITE);
    WaitForSingleObject(th2, INFINITE);
    WaitForSingleObject(th3, INFINITE);

    My_DeleteCriticalSection(lpsec);
}
