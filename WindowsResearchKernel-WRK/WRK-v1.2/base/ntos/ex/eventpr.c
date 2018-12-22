/*
Copyright (c) Microsoft Corporation. All rights reserved.

You may only use this code if you agree to the terms of the Windows Research Kernel Source Code License agreement (see License.txt).
If you do not agree to the terms, do not use the code.

Module Name:
    eventpr.c

Abstract:
    This module implements the executive event pair object.  Functions are provided to create, open, waitlow, waithi, setlow, sethi, sethiwaitlo, setlowaithi.
*/

#include "exp.h"

POBJECT_TYPE ExEventPairObjectType;// Address of event pair object type descriptor.

// Structure that describes the mapping of generic access rights to object specific access rights for event pair objects.

#ifdef ALLOC_DATA_PRAGMA
#pragma const_seg("INITCONST")
#endif

const GENERIC_MAPPING ExpEventPairMapping = {
    STANDARD_RIGHTS_READ | SYNCHRONIZE,
    STANDARD_RIGHTS_WRITE | SYNCHRONIZE,
    STANDARD_RIGHTS_EXECUTE | SYNCHRONIZE,
    EVENT_PAIR_ALL_ACCESS
};

#ifdef ALLOC_DATA_PRAGMA
#pragma const_seg()
#endif

#pragma alloc_text(INIT, ExpEventPairInitialization)
#pragma alloc_text(PAGE, NtCreateEventPair)
#pragma alloc_text(PAGE, NtOpenEventPair)
#pragma alloc_text(PAGE, NtWaitLowEventPair)
#pragma alloc_text(PAGE, NtWaitHighEventPair)
#pragma alloc_text(PAGE, NtSetLowWaitHighEventPair)
#pragma alloc_text(PAGE, NtSetHighWaitLowEventPair)
#pragma alloc_text(PAGE, NtSetHighEventPair)
#pragma alloc_text(PAGE, NtSetLowEventPair)


BOOLEAN ExpEventPairInitialization(VOID)
/*
Routine Description:
    This function creates the event pair object type descriptor at system initialization and stores the address of the object type descriptor in global storage.
Return Value:
    A value of TRUE is returned if the event pair object type descriptor is successfully initialized. Otherwise a value of FALSE is returned.
*/
{
    OBJECT_TYPE_INITIALIZER ObjectTypeInitializer;
    NTSTATUS Status;
    UNICODE_STRING TypeName;

    RtlInitUnicodeString(&TypeName, L"EventPair");// Initialize string descriptor.

    // Create event object type descriptor.
    RtlZeroMemory(&ObjectTypeInitializer, sizeof(ObjectTypeInitializer));
    ObjectTypeInitializer.Length = sizeof(ObjectTypeInitializer);
    ObjectTypeInitializer.InvalidAttributes = OBJ_OPENLINK;
    ObjectTypeInitializer.GenericMapping = ExpEventPairMapping;
    ObjectTypeInitializer.PoolType = NonPagedPool;
    ObjectTypeInitializer.DefaultNonPagedPoolCharge = sizeof(EEVENT_PAIR);
    ObjectTypeInitializer.ValidAccessMask = EVENT_PAIR_ALL_ACCESS;
    ObjectTypeInitializer.UseDefaultObject = TRUE;
    Status = ObCreateObjectType(&TypeName, &ObjectTypeInitializer, (PSECURITY_DESCRIPTOR)NULL, &ExEventPairObjectType);

    // If the event pair object type descriptor was successfully created, then return a value of TRUE. Otherwise return a value of FALSE.
    return (BOOLEAN)(NT_SUCCESS(Status));
}


NTSTATUS NtCreateEventPair(__out PHANDLE EventPairHandle, __in ACCESS_MASK DesiredAccess, __in_opt POBJECT_ATTRIBUTES ObjectAttributes)
/*
Routine Description:
    This function creates an event pair object, sets it initial state, and opens a handle to the object with the specified desired access.
Arguments:
    EventPairHandle - Supplies a pointer to a variable that will receive the event pair object handle.
    DesiredAccess - Supplies the desired types of access for the event pair object.
    ObjectAttributes - Supplies a pointer to an object attributes structure.
*/
{
    PEEVENT_PAIR EventPair;
    HANDLE Handle;
    KPROCESSOR_MODE PreviousMode;
    NTSTATUS Status;

    // Get previous processor mode and probe output handle address if necessary.
    PreviousMode = KeGetPreviousMode();
    if (PreviousMode != KernelMode) {
        try {
            ProbeForWriteHandle(EventPairHandle);
        } except(EXCEPTION_EXECUTE_HANDLER)
        {
            return GetExceptionCode();
        }
    }

    // Allocate event object.
    Status = ObCreateObject(PreviousMode, ExEventPairObjectType, ObjectAttributes, PreviousMode, NULL, sizeof(EEVENT_PAIR), 0, 0, &EventPair);

    // If the event pair object was successfully allocated, 
    // then initialize the event pair object and attempt to insert the event pair object in the current process' handle table.
    if (NT_SUCCESS(Status)) {
        KeInitializeEventPair(&EventPair->KernelEventPair);
        Status = ObInsertObject((PVOID)EventPair, NULL, DesiredAccess, 0, NULL, &Handle);

        // If the event pair object was successfully inserted in the current process' handle table, then attempt to write the event pair object handle value.
        // If the write attempt fails, then do not report an error.
        // When the caller attempts to access the handle value, an access violation will occur.
        if (NT_SUCCESS(Status)) {
            if (PreviousMode != KernelMode) {
                try {
                    *EventPairHandle = Handle;
                } except(EXCEPTION_EXECUTE_HANDLER)
                {
                    NOTHING;
                }
            } else {
                *EventPairHandle = Handle;
            }
        }
    }

    return Status;// Return service status.
}


NTSTATUS NtOpenEventPair(__out PHANDLE EventPairHandle, __in ACCESS_MASK DesiredAccess, __in POBJECT_ATTRIBUTES ObjectAttributes)
/*
Routine Description:
    This function opens a handle to an event pair object with the specified desired access.
Arguments:
    EventPairHandle - Supplies a pointer to a variable that will receive the event pair object handle.
    DesiredAccess - Supplies the desired types of access for the event pair object.
    ObjectAttributes - Supplies a pointer to an object attributes structure.
*/
{
    HANDLE Handle;
    KPROCESSOR_MODE PreviousMode;
    NTSTATUS Status;

    // Get previous processor mode and probe output handle address if necessary.
    PreviousMode = KeGetPreviousMode();
    if (PreviousMode != KernelMode) {
        try {
            ProbeForWriteHandle(EventPairHandle);
        } except(ExSystemExceptionFilter())
        {
            return GetExceptionCode();
        }
    }

    // Open handle to the event pair object with the specified desired access.
    Status = ObOpenObjectByName(ObjectAttributes, ExEventPairObjectType, PreviousMode, NULL, DesiredAccess, NULL, &Handle);

    // If the open was successful, then attempt to write the event pair object handle value.
    // If the write attempt fails, then do not report an error.
    // When the caller attempts to access the handle value, an access violation will occur.
    if (NT_SUCCESS(Status)) {
        if (PreviousMode != KernelMode) {
            try {
                *EventPairHandle = Handle;
            } except(EXCEPTION_EXECUTE_HANDLER)
            {
                NOTHING;
            }
        } else {
            *EventPairHandle = Handle;
        }
    }

    return Status;// Return service status.
}


NTSTATUS NtWaitLowEventPair(__in HANDLE EventPairHandle)
/*
Routine Description:
    This function waits on the low event of an event pair object.
Arguments:
    EventPairHandle - Supplies a handle to an event pair object.
*/
{
    PEEVENT_PAIR EventPair;
    KPROCESSOR_MODE PreviousMode;
    NTSTATUS Status;

    PreviousMode = KeGetPreviousMode();
    Status = ObReferenceObjectByHandle(EventPairHandle, SYNCHRONIZE, ExEventPairObjectType, PreviousMode, &EventPair, NULL); // Reference event pair object by handle.
    if (NT_SUCCESS(Status)) {// If the reference was successful, then wait on the Low event of the event pair.
        Status = KeWaitForLowEventPair(&EventPair->KernelEventPair, PreviousMode, FALSE, NULL);
        ObDereferenceObject(EventPair);
    }

    return Status;// Return service status.
}


NTSTATUS NtWaitHighEventPair(__in HANDLE EventPairHandle)
/*
Routine Description:
    This function waits on the high event of an event pair object.
Arguments:
    EventPairHandle - Supplies a handle to an event pair object.
*/
{
    PEEVENT_PAIR EventPair;
    KPROCESSOR_MODE PreviousMode;
    NTSTATUS Status;

    PreviousMode = KeGetPreviousMode();
    Status = ObReferenceObjectByHandle(EventPairHandle, SYNCHRONIZE, ExEventPairObjectType, PreviousMode, &EventPair, NULL); // Reference event pair object by handle.
    if (NT_SUCCESS(Status)) {// If the reference was successful, then wait on the Low event of the event pair.
        Status = KeWaitForHighEventPair(&EventPair->KernelEventPair, PreviousMode, FALSE, NULL);
        ObDereferenceObject(EventPair);
    }

    return Status;// Return service status.
}


NTSTATUS NtSetLowWaitHighEventPair(__in HANDLE EventPairHandle)
/*
Routine Description:
    This function sets the low event of an event pair and then waits on the high event of an event pair object.
Arguments:
    EventPairHandle - Supplies a handle to an event pair object.
*/
{
    PEEVENT_PAIR EventPair;
    KPROCESSOR_MODE PreviousMode;
    NTSTATUS Status;

    PreviousMode = KeGetPreviousMode();
    Status = ObReferenceObjectByHandle(EventPairHandle, SYNCHRONIZE, ExEventPairObjectType, PreviousMode, &EventPair, NULL); // Reference event pair object by handle.
    if (NT_SUCCESS(Status)) { // If the reference was successful, then wait on the Low event of the event pair.
        Status = KeSetLowWaitHighEventPair(&EventPair->KernelEventPair, PreviousMode);
        ObDereferenceObject(EventPair);
    }

    return Status;// Return service status.
}


NTSTATUS NtSetHighWaitLowEventPair(__in HANDLE EventPairHandle)
/*
Routine Description:
    This function sets the high event of an event pair and then waits on the low event of an event pair object.
Arguments:
    EventPairHandle - Supplies a handle to an event pair object.
*/
{
    PEEVENT_PAIR EventPair;
    KPROCESSOR_MODE PreviousMode;
    NTSTATUS Status;

    PreviousMode = KeGetPreviousMode();
    Status = ObReferenceObjectByHandle(EventPairHandle, SYNCHRONIZE, ExEventPairObjectType, PreviousMode, &EventPair, NULL);// Reference event pair object by handle.
    if (NT_SUCCESS(Status)) {// If the reference was successful, then wait on the Low event of the event pair.
        Status = KeSetHighWaitLowEventPair(&EventPair->KernelEventPair, PreviousMode);
        ObDereferenceObject(EventPair);
    }

    return Status;// Return service status.
}


NTSTATUS NtSetLowEventPair(__in HANDLE EventPairHandle)
/*
Routine Description:
    This function sets the low event of an event pair object.
Arguments:
    EventPairHandle - Supplies a handle to an event pair object.
*/
{
    PEEVENT_PAIR EventPair;
    KPROCESSOR_MODE PreviousMode;
    NTSTATUS Status;

    PreviousMode = KeGetPreviousMode();
    Status = ObReferenceObjectByHandle(EventPairHandle, SYNCHRONIZE, ExEventPairObjectType, PreviousMode, &EventPair, NULL);// Reference event pair object by handle.
    if (NT_SUCCESS(Status)) {// If the reference was successful, then wait on the Low event of the event pair.
        KeSetLowEventPair(&EventPair->KernelEventPair, EVENT_PAIR_INCREMENT, FALSE);
        ObDereferenceObject(EventPair);
    }

    return Status;// Return service status.
}


NTSTATUS NtSetHighEventPair(__in HANDLE EventPairHandle)
/*
Routine Description:
    This function sets the high event of an event pair object.
Arguments:
    EventPairHandle - Supplies a handle to an event pair object.
*/
{
    PEEVENT_PAIR EventPair;
    KPROCESSOR_MODE PreviousMode;
    NTSTATUS Status;

    PreviousMode = KeGetPreviousMode();
    Status = ObReferenceObjectByHandle(EventPairHandle, SYNCHRONIZE, ExEventPairObjectType, PreviousMode, &EventPair, NULL); // Reference event pair object by handle.
    if (NT_SUCCESS(Status)) {// If the reference was successful, then wait on the Low event of the event pair.
        KeSetHighEventPair(&EventPair->KernelEventPair, EVENT_PAIR_INCREMENT, FALSE);
        ObDereferenceObject(EventPair);
    }

    return Status;// Return service status.
}