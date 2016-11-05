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
 Revision Information:
 $File name:  /ST8DD_User/src/ST8DD_UserTest.c/ST8DD_SequencerTest.c $
 -------------------------------------------------------------------------
 -------------------------------------------------------------------------
 Info:
 Copyright juansolsona 2016 11:41:19
 ------------------------------------------------------------------------- */
/******************************************************************************
 * This file implements some tests for the ST8DD sequencer operations
 ******************************************************************************/

/*****************************************************************************/
/*                                 INCLUDES                                  */
/*****************************************************************************/
#include <time.h>
#include "gtest/gtest.h"
#include "ST8DD_GPIO_HAL.h"
#include "ST8DD_Sequencer.h"
#include "ST8DD_Types.h"
#include "ST8DD_Log.h"
/*****************************************************************************/
/*                         PRIVATE MACROS DEFINITONS                         */
/*****************************************************************************/
#define NUM_RUNS 100
#define S_TO_NS(X) ((X) * 1000000000ULL)
#define GET_NS_TIME(X) (S_TO_NS((X).tv_sec) + (X).tv_nsec)
/*****************************************************************************/
/*                         PRIVATE TYPES DEFINITIONS                         */
/*****************************************************************************/
/*****************************************************************************/
/*                            PRIVATE VARIABLES                              */
/*****************************************************************************/
/*****************************************************************************/
/*                       PRIVATE FUNCTIONS PROTOTYPES                        */
/*****************************************************************************/
TEST (BasicSequencer,Sequencer_Tests)
{
    extern ST8_GPIO_Interface ST8_RPI_GPIO_Interface;
    ST8DD_ErrorCodes ErrCode;
    ST8DD_GPIO_Handle HandleA;
    ST8DD_GPIO_Handle HandleB;
    ST8DD_Sequence *Sequence;
    unsigned long long StartTime;
    unsigned long long EndTime;
    unsigned long long DeltaTime;
    struct timespec tstart, tend;
    unsigned long long AccumTime = 0;

    ErrCode = ST8_RPI_GPIO_Interface.RequestGPIO(1, &HandleA);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
    if (ST8DD_Err_NoError == ErrCode)
    {
        LogInfo("Now requesting second instance of a GPIO");
        ErrCode = ST8_RPI_GPIO_Interface.RequestGPIO(2, &HandleB);
        ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
        if (ST8DD_Err_NoError != ErrCode)
        {
            LogError("Error testing GPIO %d", ErrCode);
        }
        else
        {
            // Now let's create a sequence
            ErrCode=ST8DD_Sequence_CreateSequence(&Sequence,&ST8_RPI_GPIO_Interface);
            ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
            if (ST8DD_Err_NoError != ErrCode)
            {
                LogInfo("Now requesting second instance of a GPIO");
            }
            else
            {
                u8 Ordinal;
                u32 j;
                ST8DD_GPIO_Level Level;
                u8 CurrentOrdinal=0;
                ErrCode=ST8DD_Sequence_InserGPIO_DirChange_Tail(Sequence,&Ordinal,HandleA,ST8DD_GPIO_Sense_Out,0);
                ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
                ASSERT_EQ(Ordinal, ++CurrentOrdinal);
                ErrCode=ST8DD_Sequence_InserGPIO_DirChange_Tail(Sequence,&Ordinal,HandleB,ST8DD_GPIO_Sense_Out,0);
                ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
                ASSERT_EQ(Ordinal, ++CurrentOrdinal);
                ErrCode=ST8DD_Sequence_InserGPIO_ValChange_Tail(Sequence,&Ordinal,HandleA,ST8DD_GPIO_Level_Low,0);
                ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
                ASSERT_EQ(Ordinal, ++CurrentOrdinal);
                ErrCode=ST8DD_Sequence_InserGPIO_ValChange_Tail(Sequence,&Ordinal,HandleB,ST8DD_GPIO_Level_High,0);
                ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
                ASSERT_EQ(Ordinal, ++CurrentOrdinal);
                ErrCode=ST8DD_Sequence_InserGPIO_DirChange_Tail(Sequence,&Ordinal,HandleA,ST8DD_GPIO_Sense_In,0);
                ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
                ASSERT_EQ(Ordinal, ++CurrentOrdinal);
                ErrCode=ST8DD_Sequence_InserGPIO_ValRead_Tail(Sequence,&Ordinal,HandleA,0);
                ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
                ASSERT_EQ(Ordinal, ++CurrentOrdinal);
                for(j=0;j<NUM_RUNS;j++)
                {
                    clock_gettime(CLOCK_MONOTONIC, &tstart);
                    ErrCode=ST8DD_Sequence_ExecuteSequence(Sequence);
                    clock_gettime(CLOCK_MONOTONIC, &tend);
                    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
                    StartTime = GET_NS_TIME(tstart);
                    EndTime = GET_NS_TIME(tend);
                    DeltaTime = EndTime - StartTime;
                    AccumTime += DeltaTime;
                }

                ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
                ErrCode=ST8DD_Sequence_ReadExecutionResult(Sequence,CurrentOrdinal-1,&Level);
                ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
                ASSERT_EQ(Level, ST8DD_GPIO_Level_High);
                ErrCode=ST8DD_Sequence_DestroySequence(Sequence);

            }
        }
    }
    LogInfo("Now releasing the GPIO A");
    ErrCode = ST8_RPI_GPIO_Interface.ReleaseGPIO(HandleA);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
    LogInfo("Now releasing the GPIO B");
    ErrCode = ST8_RPI_GPIO_Interface.ReleaseGPIO(HandleB);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
    LogInfo("Time to execute a sequence GPIO %f (us)", ((double)AccumTime/(double)NUM_RUNS / 1000.));
}

/*****************************************************************************/
/*                     PRIVATE FUNCTIONS IMPLEMENTATIONS                     */
/*****************************************************************************/
/*****************************************************************************/
/*                    PUBLIC FUNCTIONS IMPLEMENTATIONS                       */
/*****************************************************************************/
/*****************************************************************************/
/*                                   END                                     */
/*****************************************************************************/
