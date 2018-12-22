/* AP 10-mar-05 heaps.c - producer/consumer app using multiple heaps    */
/* Example solution to assignment 5.2.3                 */
/* Windows Operating System Internals                   */

# include <windows.h>
# include <stdio.h>

/* the conversion routine */
/* implementing business logic for tr is left as exercise */
char Convert(char inp) { return inp; }

DWORD WINAPI Producer(LPVOID lpParam);
DWORD WINAPI Converter(LPVOID lpParam);
DWORD WINAPI Consumer(LPVOID lpParam);

static HANDLE hProducer, hConverter, hConsumer;
static HANDLE inpHeap, outpHeap;

static int producer_finished, converter_finished;

typedef struct _charstruct
{
    char c;
    struct _charstruct * next;
} *charstructp;

static charstructp inp;
static charstructp outp;


main()
{
    inpHeap = HeapCreate(HEAP_NO_SERIALIZE, 1024, 0);
    outpHeap = HeapCreate(HEAP_NO_SERIALIZE, 1024, 0);
    if (!inpHeap || !outpHeap)
        fprintf(stderr, "error creating heaps\n"), exit(1);

    hProducer = CreateThread(NULL, 0, Producer, NULL, CREATE_SUSPENDED, NULL);
    hConverter = CreateThread(NULL, 0, Converter, NULL, CREATE_SUSPENDED, NULL);
    hConsumer = CreateThread(NULL, 0, Consumer, NULL, CREATE_SUSPENDED, NULL);
    if (!hProducer || !hConverter || !hConsumer)
        fprintf(stderr, "error creating threads\n"), exit(2);

    ResumeThread(hProducer);

    WaitForSingleObject(hProducer, INFINITE);
    WaitForSingleObject(hConverter, INFINITE);
    WaitForSingleObject(hConsumer, INFINITE);
    exit(0);
}


/* producer reads stdin and stores data on first heap       */
/* resumes converter as soon as data has arrived        */
DWORD WINAPI Producer(LPVOID lpParam)
{
    charstructp rdp, cp;

    while (!feof(stdin)) {
        cp = HeapAlloc(inpHeap, 0, sizeof(struct _charstruct));
        if (!cp)
            fprintf(stderr, "HeapAlloc inpHeap failed\n"), exit(3);
        cp->c = getc(stdin);
        cp->next = NULL;

        if (!inp) {
            inp = cp;
            rdp = cp;
        } else {
            rdp->next = cp;
            rdp = rdp->next;
        }

        ResumeThread(hConverter);
    }

    producer_finished = 1;
    ResumeThread(hConverter);
    ExitThread(0);
}


/* retrieve data from first heap; call conversion routine   */
/* store data on second heap                    */
/* this routine is executed in a seprate thread that is resumed */
/* only called after inp has been initialized           */
/* resumes consumer as soon as data has been converted      */
DWORD WINAPI Converter(LPVOID lpParam)
{
    charstructp wrp, rdp, cp;

    rdp = inp;
    do {
        cp = HeapAlloc(outpHeap, 0, sizeof(struct _charstruct));
        if (!cp)
            fprintf(stderr, "HeapAlloc outpHeap failed\n"), exit(4);
        cp->c = Convert(rdp->c);
        cp->next = NULL;

        if (!outp) {
            outp = cp;
            wrp = cp;
        } else {
            wrp->next = cp;
            wrp = wrp->next;
        }

        ResumeThread(hConsumer);
        if (rdp->next)
            rdp = rdp->next;
        else
            SuspendThread(hConverter);
    } while (!producer_finished || rdp->next);

    HeapDestroy(inpHeap);
    converter_finished = 1;
    ResumeThread(hConsumer);
    ExitThread(0);
}


/* read second heap; write converted data to stdout          */
/* this routine is executed in a separate thread that is resumed */
/* only after outp has been initialized              */
DWORD WINAPI Consumer(LPVOID lpParam)
{
    charstructp wrp;

    wrp = outp;
    do {
        if (putc(wrp->c, stdout) == EOF)
            fprintf(stderr, "putc(stdout) failed\n"), exit(5);

        if (wrp->next)
            wrp = wrp->next;
        else
            SuspendThread(hConsumer);
    } while (!converter_finished || wrp->next);

    HeapDestroy(outpHeap);
    ExitThread(0);
}