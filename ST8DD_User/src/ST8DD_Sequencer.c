/*
 This file is part of ST8DD_User.

 ST8DD_User is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ST8DD_User is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with ST8DD_User.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------
 Info:
 Copyright juansolsona 31 oct. 2016
 ------------------------------------------------------------------------- */
/******************************************************************************
 * This file implements ......
 ******************************************************************************/

/*****************************************************************************/
/*                                 INCLUDES                                  */
/*****************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ST8DD_Log.h"
#include "ST8DD_Types.h"
#include "ST8DD_Error.h"
#include "ST8DD_GPIO_HAL.h"
#include "ST8DD_Sequencer.h"
#ifdef __cplusplus
extern "C"
{
#endif
/*****************************************************************************/
/*                         PRIVATE MACROS DEFINITONS                         */
/*****************************************************************************/
/*****************************************************************************/
/*                         PRIVATE TYPES DEFINITIONS                         */
/*****************************************************************************/
/**
 * Enumeration for all the operations that the sequencer may perform
 */
typedef enum enST8DD_Seq_OperationType
{
    Operation_SetGPIO_Dir, Operation_SetGPIO_Val, Operation_GetGPIO_Val, Operation_WaitTime,
} ST8DD_Seq_OperationType;

/**
 * Structure that defines one action to be performed by the sequencer
 */
typedef struct stST8DD_OperationEntry
{
ST8DD_Seq_OperationType Operation;
u64 DelayBeforeMs;
ST8DD_GPIO_Handle Handle;
struct stST8DD_OperationEntry *Next;
union
{
ST8DD_GPIO_Level Level;
ST8DD_GPIO_Direction Dir;
} Argument;
} ST8DD_OperationEntry;
/*****************************************************************************/
/*                            PRIVATE VARIABLES                              */
/*****************************************************************************/
/*****************************************************************************/
/*                       PRIVATE FUNCTIONS PROTOTYPES                        */
/*****************************************************************************/
static ST8DD_ErrorCodes ST8DD_SeqAllocateSequenceEntry(ST8DD_OperationEntry **SequenceEntry);
static void STDD_SeqPutInTailElement(ST8DD_Sequence *Sequence, ST8DD_OperationEntry *SequenceEntry);

static ST8DD_ErrorCodes      ST8DD_SeqPerformOperation_SetGPIO_Dir(ST8DD_Sequence *Sequence, ST8DD_OperationEntry *SequenceEntry);
static ST8DD_ErrorCodes      ST8DD_SeqPerformOperation_SetGPIO_Val(ST8DD_Sequence *Sequence, ST8DD_OperationEntry *SequenceEntry);
static ST8DD_ErrorCodes      ST8DD_SeqPerformOperation_GetGPIO_Val(ST8DD_Sequence *Sequence, ST8DD_OperationEntry *SequenceEntry);
static ST8DD_ErrorCodes      ST8DD_SeqPerformOperation_WaitTime(ST8DD_Sequence *Sequence, ST8DD_OperationEntry *SequenceEntry);
static ST8DD_OperationEntry *ST8DD_SeqGetEntryByIndex(ST8DD_Sequence *Sequence,u8 Index );

/*****************************************************************************/
/*                     PRIVATE FUNCTIONS IMPLEMENTATIONS                     */
/*****************************************************************************/
/**
 * This function returns a pointer to a sequence entry based on an index, since is static it does not perform any
 * checking on types or range
 * @param Sequence
 * @param Index
 * @return A pointer to the index entry.
 */
static ST8DD_OperationEntry *ST8DD_SeqGetEntryByIndex(ST8DD_Sequence *Sequence,u8 Index )
{
    ST8DD_OperationEntry *RetVal=Sequence->Head;
    while(Index--)
    {
        RetVal=RetVal->Next;
    }
    return RetVal;
}

/* This function allocates and intializes a sequence entry */
static ST8DD_ErrorCodes ST8DD_SeqAllocateSequenceEntry(ST8DD_OperationEntry **SequenceEntry)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    ST8DD_OperationEntry *NewSequenceEntry;
    if (NULL == SequenceEntry)
    {
        RetValue = ST8DD_Err_InvalidArgument;
        LogError("Invalid Sequence pointer");
    }
    else
    {
        NewSequenceEntry = malloc(sizeof(*NewSequenceEntry));
        if (NULL == NewSequenceEntry)
        {
            RetValue = ST8DD_Err_OutOfMemory;
        }
        else
        {
            memset(NewSequenceEntry, 0, sizeof(*NewSequenceEntry));
            *SequenceEntry = NewSequenceEntry;

        }
    }
    return RetValue;

}
/*****************************************************************************/
static void STDD_SeqPutInTailElement(ST8DD_Sequence *Sequence, ST8DD_OperationEntry *SequenceEntry)
{
    if (NULL == Sequence->Head)
    {
        Sequence->Head = Sequence->Tail = SequenceEntry;
    }
    else
    {
        Sequence->Tail->Next = SequenceEntry;
        Sequence->Tail = SequenceEntry;
    }
}
/*****************************************************************************/
static inline void ST8DD_SeqWaitTime(u32 Milliseconds)
{
    struct timespec ts;
    ts.tv_sec = Milliseconds / 1000;
    ts.tv_nsec = (Milliseconds % 1000) * 1000000;
    do
    {
        NULL;
    } while (0 != nanosleep(&ts, &ts));
}
/*****************************************************************************/
static ST8DD_ErrorCodes ST8DD_SeqPerformOperation_SetGPIO_Dir(ST8DD_Sequence *Sequence, ST8DD_OperationEntry *SequenceEntry)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (SequenceEntry->DelayBeforeMs > 0)
        ST8DD_SeqWaitTime(SequenceEntry->DelayBeforeMs);
    RetValue = Sequence->GPIO_Instance->SetDirectionGPIO(SequenceEntry->Handle, SequenceEntry->Argument.Dir);
    return RetValue;
}
/*****************************************************************************/
static ST8DD_ErrorCodes ST8DD_SeqPerformOperation_SetGPIO_Val(ST8DD_Sequence *Sequence, ST8DD_OperationEntry *SequenceEntry)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (SequenceEntry->DelayBeforeMs > 0)
        ST8DD_SeqWaitTime(SequenceEntry->DelayBeforeMs);
    RetValue = Sequence->GPIO_Instance->SetValueGPIO(SequenceEntry->Handle, SequenceEntry->Argument.Level);
    return RetValue;
}
/*****************************************************************************/
static ST8DD_ErrorCodes ST8DD_SeqPerformOperation_GetGPIO_Val(ST8DD_Sequence *Sequence, ST8DD_OperationEntry *SequenceEntry)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (SequenceEntry->DelayBeforeMs > 0)
        ST8DD_SeqWaitTime(SequenceEntry->DelayBeforeMs);
    RetValue = Sequence->GPIO_Instance->GetValueGPIO(SequenceEntry->Handle, &SequenceEntry->Argument.Level);
    return RetValue;
}
/*****************************************************************************/
static ST8DD_ErrorCodes ST8DD_SeqPerformOperation_WaitTime(ST8DD_Sequence *Sequence, ST8DD_OperationEntry *SequenceEntry)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (SequenceEntry->DelayBeforeMs > 0)
        ST8DD_SeqWaitTime(SequenceEntry->DelayBeforeMs);
    return RetValue;
}
/*****************************************************************************/
/*****************************************************************************/
/*                     PUBLIC FUNCTIONS IMPLEMENTATIONS                      */
/*****************************************************************************/
/**
 * This function allocates a new sequence entry and initializes its values.
 * @param Sequence
 * @return
 */
ST8DD_ErrorCodes ST8DD_Sequence_CreateSequence(ST8DD_Sequence **Sequence, ST8_GPIO_Interface *GPIO_Instance)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    ST8DD_Sequence *NewSequence;
    if (NULL == Sequence)
    {
        RetValue = ST8DD_Err_InvalidArgument;
        LogError("Invalid Sequence pointer");
    }
    else
    {
        NewSequence = malloc(sizeof(*NewSequence));
        if (NULL == NewSequence)
        {
            RetValue = ST8DD_Err_OutOfMemory;
        }
        else
        {
            memset(NewSequence, 0, sizeof(*NewSequence));
            NewSequence->GPIO_Instance = GPIO_Instance;
            *Sequence = NewSequence;

        }
    }
    return RetValue;
}
/*****************************************************************************/
ST8DD_ErrorCodes ST8DD_Sequence_InserGPIO_ValChange_Tail(ST8DD_Sequence *Sequence, u32 *Ordinal, ST8DD_GPIO_Handle Handle, ST8DD_GPIO_Level Level, u32 WaitMilliseconds)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (NULL == Sequence)
    {
        RetValue = ST8DD_Err_InvalidArgument;
        LogError("Invalid Sequence pointer");
    }
    else
    {
        ST8DD_OperationEntry *Entry = NULL;
        RetValue = ST8DD_SeqAllocateSequenceEntry(&Entry);
        if (ST8DD_Err_NoError != RetValue)
        {
            LogError("Invalid out of memory");
        }
        else
        {
            Entry->Argument.Level = Level;
            Entry->DelayBeforeMs = WaitMilliseconds;
            Entry->Handle = Handle;
            Entry->Operation = Operation_SetGPIO_Val;
            STDD_SeqPutInTailElement(Sequence, Entry);
            *Ordinal = ++Sequence->NumberOfOperations;
        }
    }

    return RetValue;

}
/*****************************************************************************/
ST8DD_ErrorCodes ST8DD_Sequence_InserGPIO_DirChange_Tail(ST8DD_Sequence *Sequence, u32 *Ordinal, ST8DD_GPIO_Handle Handle, ST8DD_GPIO_Direction Dir, u32 WaitMilliseconds)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (NULL == Sequence)
    {
        RetValue = ST8DD_Err_InvalidArgument;
        LogError("Invalid Sequence pointer");
    }
    else
    {
        ST8DD_OperationEntry *Entry = NULL;
        RetValue = ST8DD_SeqAllocateSequenceEntry(&Entry);
        if (ST8DD_Err_NoError != RetValue)
        {
            LogError("Invalid out of memory");
        }
        else
        {
            Entry->Argument.Dir = Dir;
            Entry->DelayBeforeMs = WaitMilliseconds;
            Entry->Handle = Handle;
            Entry->Operation = Operation_SetGPIO_Dir;
            STDD_SeqPutInTailElement(Sequence, Entry);
            *Ordinal = ++Sequence->NumberOfOperations;
        }
    }

    return RetValue;

}

/*****************************************************************************/
ST8DD_ErrorCodes ST8DD_Sequence_InserGPIO_ValRead_Tail(ST8DD_Sequence *Sequence, u32 *Ordinal, ST8DD_GPIO_Handle Handle, u32 WaitMilliseconds)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (NULL == Sequence)
    {
        RetValue = ST8DD_Err_InvalidArgument;
        LogError("Invalid Sequence pointer");
    }
    else
    {
        ST8DD_OperationEntry *Entry = NULL;
        RetValue = ST8DD_SeqAllocateSequenceEntry(&Entry);
        if (ST8DD_Err_NoError != RetValue)
        {
            LogError("Invalid out of memory");
        }
        else
        {
            Entry->DelayBeforeMs = WaitMilliseconds;
            Entry->Handle = Handle;
            Entry->Operation = Operation_GetGPIO_Val;
            STDD_SeqPutInTailElement(Sequence, Entry);
            *Ordinal = ++Sequence->NumberOfOperations;
        }
    }

    return RetValue;
}

/*****************************************************************************/
ST8DD_ErrorCodes ST8DD_Sequence_ExecuteSequence(ST8DD_Sequence *Sequence)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (NULL == Sequence)
    {
        RetValue = ST8DD_Err_InvalidArgument;
        LogError("Invalid Sequence pointer");
    }
    else
    {
        ST8DD_OperationEntry *Entry = Sequence->Head;
        while (NULL != Entry)
        {
            switch (Entry->Operation)
            {
                case Operation_SetGPIO_Dir:
                    RetValue = ST8DD_SeqPerformOperation_SetGPIO_Dir(Sequence, Entry);
                    break;
                case Operation_SetGPIO_Val:
                    RetValue = ST8DD_SeqPerformOperation_SetGPIO_Val(Sequence, Entry);
                    break;
                case Operation_GetGPIO_Val:
                    RetValue = ST8DD_SeqPerformOperation_GetGPIO_Val(Sequence, Entry);
                    break;
                case Operation_WaitTime:
                    RetValue = ST8DD_SeqPerformOperation_WaitTime(Sequence, Entry);
                    break;
                default:
                    break;
            }
            if (ST8DD_Err_NoError == RetValue)
            {
                Entry = Entry->Next;
            }
            else
            {
                break;
            }
        }

    }
    return RetValue;
}
/*****************************************************************************/
/**
 * This function allows to retrieve the value of a GPIO value after the execution
 * of the sequence, mainly used to get inputs values.
 * @param Sequence
 * @param Index
 * @param Level
 * @return
 */
/*****************************************************************************/
ST8DD_ErrorCodes ST8DD_Sequence_ReadExecutionResult(ST8DD_Sequence *Sequence, u8 Index, ST8DD_GPIO_Level *Level)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (NULL == Sequence)
    {
        RetValue = ST8DD_Err_InvalidArgument;
        LogError("Invalid Sequence pointer");
    }
    else
    {
        if (NULL == Level)
        {
            RetValue = ST8DD_Err_InvalidArgument;
            LogError("Invalid level pointer value");

        }
        else
        {
            if (Index >= Sequence->NumberOfOperations)
            {
                RetValue = ST8DD_Err_InvalidArgument;
                LogError("Invalid Sequence index");
            }
            else
            {
                *Level=ST8DD_SeqGetEntryByIndex(Sequence,Index)->Argument.Level;
            }
        }
    }

    return RetValue;
}

/*****************************************************************************/
/**
 * This function wipes all the entries on a sequence and the sequence pointer
 * itself.
 * @param Sequence
 * @return
 */
/*****************************************************************************/
ST8DD_ErrorCodes ST8DD_Sequence_DestrySequence(ST8DD_Sequence *Sequence)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (NULL == Sequence)
    {
        RetValue = ST8DD_Err_InvalidArgument;
        LogError("Invalid Sequence pointer");
    }
    else
    {
        ST8DD_OperationEntry *CurrentEntry=Sequence->Head;
        while(CurrentEntry)
        {
            ST8DD_OperationEntry *TmpVal=CurrentEntry;
            CurrentEntry=CurrentEntry->Next;
            free(TmpVal);
        }
        free(Sequence);
    }

    return RetValue;
}
/*****************************************************************************/
/*                                   END                                     */
/*****************************************************************************/
