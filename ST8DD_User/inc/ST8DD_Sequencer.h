/*
    This file is part of ST8DD_User.

    Foobar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ST8DD_User is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ST8DD_User.  If not, see <http://www.gnu.org/licenses/>.

    This file defines the sequencer data types and interfaces. A sequencer
    is an entity that perform operations with the GPIOs following a script
    on a strong time.
 */

#ifndef ST8DD_SEQUENCER_H_
#define ST8DD_SEQUENCER_H_
/*****************************************************************************/
/*                                 INCLUDES                                  */
/*****************************************************************************/

#include "ST8DD_Types.h"
#include "ST8DD_Error.h"
#include "ST8DD_GPIO_HAL.h"
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/*                         PUBLIC MACROS DEFINITONS                          */
/*****************************************************************************/
/*****************************************************************************/
/*                         PUBLIC TYPES DEFINITIONS                          */
/*****************************************************************************/
/**
 * Structure that defines one action to be performed by the sequencer
 */
struct stST8DD_OperationEntry;
/**
 * A sequence of operations
 */
typedef struct stST8DD_Sequence
{
    ST8_GPIO_Interface *GPIO_Instance;
    u32 NumberOfOperations;
    struct stST8DD_OperationEntry *Head;
    struct stST8DD_OperationEntry *Tail;
} ST8DD_Sequence;




/*****************************************************************************/
/*                             PUBLIC VARIABLES                              */
/*****************************************************************************/
/*****************************************************************************/
/*                        PUBLIC FUNCTIONS PROTOTYPES                        */
/*****************************************************************************/
ST8DD_ErrorCodes ST8DD_Sequence_CreateSequence(ST8DD_Sequence **Sequence,ST8_GPIO_Interface *GPIO_Instance);
ST8DD_ErrorCodes ST8DD_Sequence_DestroySequence(ST8DD_Sequence *Sequence);
ST8DD_ErrorCodes ST8DD_Sequence_InserGPIO_ValChange_Tail(ST8DD_Sequence *Sequence,u8 *Ordinal,ST8DD_GPIO_Handle Handle,ST8DD_GPIO_Level Level,  u32 WaitMilliseconds);
ST8DD_ErrorCodes ST8DD_Sequence_InserGPIO_DirChange_Tail(ST8DD_Sequence *Sequence,u8 *Ordinal,ST8DD_GPIO_Handle Handle,ST8DD_GPIO_Direction Dir,u32 WaitMilliseconds);
ST8DD_ErrorCodes ST8DD_Sequence_InserGPIO_ValRead_Tail(  ST8DD_Sequence *Sequence,u8 *Ordinal,ST8DD_GPIO_Handle Handle,                         u32 WaitMilliseconds);

ST8DD_ErrorCodes ST8DD_Sequence_ExecuteSequence(ST8DD_Sequence *Sequence);
ST8DD_ErrorCodes ST8DD_Sequence_ReadExecutionResult(ST8DD_Sequence *Sequence,u8 Index,ST8DD_GPIO_Level *Level );

/*****************************************************************************/
/*                       PUBLIC INTERFACES DECLARATIONS                      */
/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ST8DD_SEQUENCER_H_ */
