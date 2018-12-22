/* AP 18-apr-05	producer-consumer using critical sections for synch	*/

# include <stdio.h>
# include <stdlib.h>
# include <windows.h>

static CRITICAL_SECTION sec;
static LPCRITICAL_SECTION lpsec = &sec;

static int counter;
static int min_counter;
static int max_counter;

DWORD WINAPI Producer(LPVOID arg)
{
    int iter = 0;

    do {
        EnterCriticalSection(lpsec);
        counter++;
        if (counter > max_counter)
            max_counter = counter;

        printf("Producer: counter = %d\n", counter);
        LeaveCriticalSection(lpsec);
        iter++;
    } while (iter < 1000);

    return 0;
}

DWORD WINAPI Consumer(LPVOID arg)
{
    int iter = 0;

    do {
        EnterCriticalSection(lpsec);
        counter--;
        if (counter < min_counter)
            min_counter = counter;

        printf("Consumer: counter = %d\n", counter);
        LeaveCriticalSection(lpsec);
        iter++;
    } while (iter < 1000);

    return 0;
}

main()
{
    HANDLE hProducer, hConsumer;
    int arg;
    InitializeCriticalSection(lpsec);

    hProducer = CreateThread(NULL, 0, Producer, NULL, 0, NULL);
    hConsumer = CreateThread(NULL, 0, Consumer, NULL, 0, NULL);

    WaitForSingleObject(hProducer, INFINITE);
    WaitForSingleObject(hConsumer, INFINITE);

    printf("%d <= counter <= %d\n", min_counter, max_counter);

    DeleteCriticalSection(lpsec);
}
