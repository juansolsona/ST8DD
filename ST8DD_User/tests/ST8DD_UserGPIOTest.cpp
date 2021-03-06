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
 $File name:  /ST8DD_User/src/ST8DD_UserTest.c/ST8DD_UserTest.c $
 -------------------------------------------------------------------------
 -------------------------------------------------------------------------
 Info:
 Copyright juansolsona 2016 11:41:19
 ------------------------------------------------------------------------- */
/******************************************************************************
 * This file implements some tests for the ST8DD
 ******************************************************************************/

/*****************************************************************************/
/*                                 INCLUDES                                  */
/*****************************************************************************/
#include "gtest/gtest.h"
#include "ST8DD_GPIO_HAL.h"
#include "ST8DD_Types.h"
#include "ST8DD_Log.h"
/*****************************************************************************/
/*                         PRIVATE MACROS DEFINITONS                         */
/*****************************************************************************/
#define NUM_RUNS 10
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
TEST (BasicGPIO,GPIO_Tests)
{
    extern ST8_GPIO_Interface ST8_RPI_GPIO_Interface;
    ST8DD_ErrorCodes ErrCode;
    ST8DD_GPIO_Handle Handle;
    ST8DD_GPIO_Handle Handle2;
// LogLevelInfo("Enter " __func__);
    ErrCode = ST8_RPI_GPIO_Interface.RequestGPIO(1, &Handle);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
    if (ST8DD_Err_NoError == ErrCode)
    {
        LogInfo("Now releasing the GPIO");
        ErrCode = ST8_RPI_GPIO_Interface.ReleaseGPIO(Handle);
        if (ST8DD_Err_NoError != ErrCode)
        {
            LogError("Error releasing GPIO %d", ErrCode);
        }
        else
        {
            LogInfo("Now releasing second instance of a GPIO");
            ErrCode = ST8_RPI_GPIO_Interface.RequestGPIO(1, &Handle);
            if (ST8DD_Err_NoError != ErrCode)
            {
                LogError("Error testing GPIO %d", ErrCode);
            }
            else
            {
                ErrCode = ST8_RPI_GPIO_Interface.RequestGPIO(2, &Handle2);
                if (ST8DD_Err_NoError != ErrCode)
                {
                    LogError("Error requesting 2nd GPIO %d", ErrCode);
                }
                else
                {
                    ErrCode = ST8_RPI_GPIO_Interface.ReleaseGPIO(Handle2);
                    if (ST8DD_Err_NoError != ErrCode)
                    {
                        LogError("Error releasing 2nd GPIO %d", ErrCode);
                    }
                    else
                    {
                        ErrCode = ST8_RPI_GPIO_Interface.ReleaseGPIO(Handle);
                        if (ST8DD_Err_NoError != ErrCode)
                        {
                            LogError("Error releasing first GPIO %d", ErrCode);
                        }
                        else
                        {
                            /* Now, let's create a orphan GPIO that shall be released by
                             * the application destrucgor
                             */
                            ErrCode = ST8_RPI_GPIO_Interface.RequestGPIO(22, &Handle2);
                            if (ST8DD_Err_NoError != ErrCode)
                            {
                                LogError("Error requesting 2nd GPIO %d", ErrCode);
                            }
                            else
                                LogInfo("GPIO TEst OK");

                        }
                    }
                }
            }
        }
    }
}
/**
 * Test for GPIO creation / removal
 */
TEST (GPIOCreationReleaseTimes,GPIO_Tests)
{
    extern ST8_GPIO_Interface ST8_RPI_GPIO_Interface;
    ST8DD_ErrorCodes ErrCode;
    ST8DD_GPIO_Handle Handle;
    unsigned long long StartTime;
    unsigned long long EndTime;
    unsigned long long DeltaTime;
    unsigned long long AccumCreation = 0;
    unsigned long long AccumRemoval = 0;
    unsigned int run;
    struct timespec tstart, tend;
    for (run = 0; run < NUM_RUNS; run++)
    {
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        ErrCode = ST8_RPI_GPIO_Interface.RequestGPIO(1, &Handle);
        clock_gettime(CLOCK_MONOTONIC, &tend);
        ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
        StartTime = GET_NS_TIME(tstart);
        EndTime = GET_NS_TIME(tend);
        DeltaTime = EndTime - StartTime;
        AccumCreation += DeltaTime;

        clock_gettime(CLOCK_MONOTONIC, &tstart);
        ErrCode = ST8_RPI_GPIO_Interface.ReleaseGPIO(Handle);
        clock_gettime(CLOCK_MONOTONIC, &tend);
        ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
        EndTime = GET_NS_TIME(tend);
        DeltaTime = EndTime - StartTime;
        AccumRemoval += DeltaTime;
    }
    LogInfo("Average time to create a GPIO %f (us)", ((double)AccumCreation/(double)NUM_RUNS / 1000.));
    LogInfo("Average time to remove a GPIO %f (us)", ((double)AccumRemoval /(double)NUM_RUNS / 1000.));

}
TEST (GPIOCSetValuesTimes,GPIO_Tests)
{
    extern ST8_GPIO_Interface ST8_RPI_GPIO_Interface;
    ST8DD_ErrorCodes ErrCode;
    ST8DD_GPIO_Handle Handle;
    unsigned long long StartTime;
    unsigned long long EndTime;
    unsigned long long DeltaTime;
    unsigned long long AccumZeroVal = 0;
    unsigned long long AccumHighVal = 0;
    unsigned int run;
    struct timespec tstart, tend;

    ErrCode = ST8_RPI_GPIO_Interface.RequestGPIO(1, &Handle);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);

    ErrCode = ST8_RPI_GPIO_Interface.SetDirectionGPIO(Handle, ST8DD_GPIO_Sense_Out);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);

    for (run = 0; run < NUM_RUNS; run++)
    {
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        ErrCode = ST8_RPI_GPIO_Interface.SetValueGPIO(Handle, ST8DD_GPIO_Level_Low);
        clock_gettime(CLOCK_MONOTONIC, &tend);
        ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
        StartTime = GET_NS_TIME(tstart);
        EndTime = GET_NS_TIME(tend);
        DeltaTime = EndTime - StartTime;
        AccumZeroVal += DeltaTime;

        clock_gettime(CLOCK_MONOTONIC, &tstart);
        ErrCode = ST8_RPI_GPIO_Interface.SetValueGPIO(Handle, ST8DD_GPIO_Level_High);
        clock_gettime(CLOCK_MONOTONIC, &tend);
        ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
        StartTime = GET_NS_TIME(tstart);
        EndTime = GET_NS_TIME(tend);
        DeltaTime = EndTime - StartTime;
        AccumHighVal += DeltaTime;
    }

    ErrCode = ST8_RPI_GPIO_Interface.ReleaseGPIO(Handle);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);

    LogInfo("Average time to set a zero val for GPIO %f (us)", ((double)AccumZeroVal/(double)NUM_RUNS / 1000.));
    LogInfo("Average time to set a  one val for GPIO %f (us)", ((double)AccumHighVal /(double)NUM_RUNS / 1000.));

}
/**
 * Test to measure required time for reading values
 */
TEST (GPIOCGetValuesTimes,GPIO_Tests)
{
    extern ST8_GPIO_Interface ST8_RPI_GPIO_Interface;
    ST8DD_ErrorCodes ErrCode;
    ST8DD_GPIO_Handle Handle;
    unsigned long long StartTime;
    unsigned long long EndTime;
    unsigned long long DeltaTime;
    unsigned long long AccumReadTime = 0;
    unsigned int run;
    ST8DD_GPIO_Level Val;
    struct timespec tstart, tend;

    ErrCode = ST8_RPI_GPIO_Interface.RequestGPIO(1, &Handle);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);

    ErrCode = ST8_RPI_GPIO_Interface.SetDirectionGPIO(Handle, ST8DD_GPIO_Sense_In);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);

    for (run = 0; run < NUM_RUNS; run++)
    {
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        ErrCode = ST8_RPI_GPIO_Interface.GetValueGPIO(Handle, &Val);
        clock_gettime(CLOCK_MONOTONIC, &tend);
        ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
        StartTime = GET_NS_TIME(tstart);
        EndTime = GET_NS_TIME(tend);
        DeltaTime = EndTime - StartTime;
        AccumReadTime += DeltaTime;
    }

    ErrCode = ST8_RPI_GPIO_Interface.ReleaseGPIO(Handle);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);

    LogInfo("Average time to read a value from a GPIO %f (us)", ((double)AccumReadTime /(double)NUM_RUNS / 1000.));

}

/**
 * Test to measure required time for setting directions
 */
TEST (GPIOCGetDirectionTimes,GPIO_Tests)
{
    extern ST8_GPIO_Interface ST8_RPI_GPIO_Interface;
    ST8DD_ErrorCodes ErrCode;
    ST8DD_GPIO_Handle Handle;
    unsigned long long StartTime;
    unsigned long long EndTime;
    unsigned long long DeltaTime;
    unsigned long long AccumReadTime = 0;
    unsigned int run;
    ST8DD_GPIO_Direction Direction;
    struct timespec tstart, tend;

    ErrCode = ST8_RPI_GPIO_Interface.RequestGPIO(1, &Handle);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);

    ErrCode = ST8_RPI_GPIO_Interface.SetDirectionGPIO(Handle, ST8DD_GPIO_Sense_In);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);

    for (run = 0; run < NUM_RUNS; run++)
    {
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        ErrCode = ST8_RPI_GPIO_Interface.GetDirectionGPIO(Handle, &Direction);
        clock_gettime(CLOCK_MONOTONIC, &tend);
        ASSERT_EQ(ErrCode, ST8DD_Err_NoError);
        StartTime = GET_NS_TIME(tstart);
        EndTime = GET_NS_TIME(tend);
        DeltaTime = EndTime - StartTime;
        AccumReadTime += DeltaTime;
    }

    ErrCode = ST8_RPI_GPIO_Interface.ReleaseGPIO(Handle);
    ASSERT_EQ(ErrCode, ST8DD_Err_NoError);

    LogInfo("Average time to read the direction of a GPIO %f (us)", ((double)AccumReadTime /(double)NUM_RUNS / 1000.));

}

/*****************************************************************************/
/*                     PRIVATE FUNCTIONS IMPLEMENTATIONS                     */
/*****************************************************************************/
/*****************************************************************************/
/*                    PUBLIC FUNCTIONS IMPLEMENTATIONS                       */
/*****************************************************************************/
int run_tests(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*****************************************************************************/
/*                                   END                                     */
/*****************************************************************************/
