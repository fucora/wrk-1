/* AP 18-apr-05	producer-consumer using mutexes for synch	*/

# include <stdio.h>
# include <stdlib.h>
# include <windows.h>

static HANDLE mutex;

static int counter;
static int max_counter;
static int min_counter;

DWORD WINAPI Producer(LPVOID arg)
{
    int iter = 0;

    do {
        WaitForSingleObject(mutex, INFINITE);
        counter++;

        if (counter > max_counter) 
            max_counter = counter;

        printf("Producer: counter = %d\n", counter);
        ReleaseMutex(mutex);
        iter++;
    } while (iter < 1000);

    return 0;
}

DWORD WINAPI Consumer(LPVOID arg)
{
    int iter = 0;
    int i;
    DWORD ret;

    do {
        ret = WaitForSingleObject(mutex, INFINITE);
        counter--;

        if (counter < min_counter) 
            min_counter = counter;

        printf("Consumer: counter = %d\n", counter);
        iter++;
        ReleaseMutex(mutex);
    } while (iter < 1000);

    return 0;
}

main()
{
    HANDLE hProducer, hConsumer;
    HANDLE mutex = CreateMutex(NULL, FALSE, NULL);

    hProducer = CreateThread(NULL, 0, Producer, NULL, 0, NULL);
    hConsumer = CreateThread(NULL, 0, Consumer, NULL, 0, NULL);

    WaitForSingleObject(hProducer, INFINITE);
    WaitForSingleObject(hConsumer, INFINITE);

    printf("%d <= counter <= %d\n", min_counter, max_counter);

    CloseHandle(mutex);
}
