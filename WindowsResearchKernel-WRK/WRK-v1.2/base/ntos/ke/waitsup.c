/*
Copyright (c) Microsoft Corporation. All rights reserved.

You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement (see License.txt).
If you do not agree to the terms, do not use the code.

Module Name:
    waitsup.c

Abstract:
    This module contains the support routines necessary to support the generic kernel wait functions.
    Functions are provided to test if a wait can be satisfied, to satisfy a wait, and to unwwait a thread.
*/

#include "ki.h"


VOID FASTCALL KiExitDispatcher (IN KIRQL OldIrql)
/*
Routine Description:
    This function processes the deferred ready list, possibly switches to a new thread, and lowers IRQL to its previous value.
Arguments:
    OldIrql - Supplies the previous IRQL value.
*/
{
    PKTHREAD CurrentThread;
    PKTHREAD NewThread;
    BOOLEAN Pending;
    PKPRCB Prcb;

    ASSERT(KeGetCurrentIrql() == SYNCH_LEVEL);
    Prcb = KeGetCurrentPrcb();// Process the deferred ready list if the list is not empty.

#if !defined(NT_UP)
    if (Prcb->DeferredReadyListHead.Next != NULL) {
        KiProcessDeferredReadyList(Prcb);
    }
#endif

    if (OldIrql < DISPATCH_LEVEL) {// If the old IRQL is less than dispatch level, then a new thread can be dispatcher immediately.
        if (Prcb->NextThread != NULL) {// If there is a new thread selected for execution, then switch context to the new thread.
            KiAcquirePrcbLock(Prcb);
            NewThread = Prcb->NextThread;
            CurrentThread = Prcb->CurrentThread;
            KiSetContextSwapBusy(CurrentThread);
            Prcb->NextThread = NULL;
            Prcb->CurrentThread = NewThread;
            NewThread->State = Running;
            KxQueueReadyThread(CurrentThread, Prcb);
            CurrentThread->WaitIrql = OldIrql;
            Pending = KiSwapContext(CurrentThread, NewThread);
            if (Pending != FALSE) {
                KeLowerIrql(APC_LEVEL);
                KiDeliverApc(KernelMode, NULL, NULL);
                ASSERT(OldIrql == 0);
            }
        }
    } else if ((Prcb->NextThread != NULL) && (Prcb->DpcRoutineActive == FALSE)) {
        KiRequestSoftwareInterrupt(DISPATCH_LEVEL);
    }

    KeLowerIrql(OldIrql);// Lower IRQL to its previous level.
}


VOID FASTCALL KiUnwaitThread (IN PRKTHREAD Thread, IN LONG_PTR WaitStatus, IN KPRIORITY Increment)
/*
Routine Description:
    This function unwaits a thread, sets the thread's wait completion status, calculates the thread's new priority,
    and either readies the thread for execution or adds the thread to a list of threads to be readied later.
Arguments:
    Thread - Supplies a pointer to a dispatcher object of type thread.
    WaitStatus - Supplies the wait completion status.
    Increment - Supplies the priority increment that is to be applied to the thread's priority.
*/
{
    KiUnlinkThread(Thread, WaitStatus);// Unlink thread from the appropriate wait queues and set the wait completion status.

    ASSERT(Increment >= 0);// Set unwait priority adjustment parameters.

    Thread->AdjustIncrement = (SCHAR)Increment;
    Thread->AdjustReason = (UCHAR)AdjustUnwait;

    KiReadyThread(Thread);// Ready the thread for execution.
}


VOID FASTCALL KiWaitTest (IN PVOID Object, IN KPRIORITY Increment)
/*
Routine Description:
    This function tests if a wait can be satisfied when an object attains a state of signaled.
    If a wait can be satisfied, then the subject thread is unwaited with a completion status that is the WaitKey of the wait block from the object wait list.
    As many waits as possible are satisfied.
Arguments:
    Object - Supplies a pointer to a dispatcher object.
*/
{
    PKEVENT Event;
    PLIST_ENTRY ListHead;
    PRKTHREAD Thread;
    PRKWAIT_BLOCK WaitBlock;
    PLIST_ENTRY WaitEntry;
    NTSTATUS WaitStatus;

    // As long as the signal state of the specified object is Signaled and there are waiters in the object wait list, then try to satisfy a wait.
    Event = (PKEVENT)Object;
    ListHead = &Event->Header.WaitListHead;
    WaitEntry = ListHead->Flink;
    while ((Event->Header.SignalState > 0) && (WaitEntry != ListHead)) {
        WaitBlock = CONTAINING_RECORD(WaitEntry, KWAIT_BLOCK, WaitListEntry);
        Thread = WaitBlock->Thread;
        WaitStatus = STATUS_KERNEL_APC;

        // N.B. The below code only satisfies the wait for wait any types.
        //      Wait all types are satisfied in the wait code itself.
        //      This is done with a eye to the future when the dispatcher lock is split into a lock per waitable object type and a scheduling state lock.
        //      For now, a kernel APC is simulated for wait all types.
        if (WaitBlock->WaitType == WaitAny) {
            WaitStatus = (NTSTATUS)WaitBlock->WaitKey;
            KiWaitSatisfyAny((PKMUTANT)Event, Thread);
        }

        KiUnwaitThread(Thread, WaitStatus, Increment);
        WaitEntry = ListHead->Flink;
    }
}