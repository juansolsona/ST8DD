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

 *  This file implements the user space driver for the GPIO management in a Raspberry PI
 *  device
 */

/*****************************************************************************/
/*                                 INCLUDES                                  */
/*****************************************************************************/
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "malloc.h"
#include "string.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************/
/*                         PRIVATE MACROS DEFINITONS                         */
/*****************************************************************************/
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif
#include "ST8DD_Error.h"
#include "ST8DD_GPIO_HAL.h"
#include "ST8DD_Types.h"
#include "ST8DD_Log.h"

#define RSP_GPIO_SYSFS_PATH "/sys/class/gpio"
#define RSP_GPIO_EXPORTS_PATH "/export"
#define RSP_GPIO_UNEXPORTS_PATH "/unexport"
#define RSP_GPIO_INSTANCE_DIR "gpio"
#define RSP_GPIO_VALUE_FILE "value"
#define RSP_GPIO_DIRECTION_FILE "direction"

#define RSP_GPIO_DIRECTION_VAL_OUT "out"
#define RSP_GPIO_DIRECTION_VAL_IN  "in"

#define RSP_GPIO_DIR_CH_VAL_OUT 'o'
#define RSP_GPIO_DIR_CH_VAL_IN 'i'

#define RSP_GPIO_VALUE_VAL_ONE "1"
#define RSP_GPIO_VALUE_VAL_ZERO "0"

#define RSP_GPIO_VALUE_CH_VAL_ONE '1'
#define RSP_GPIO_VALUE_CH_VAL_ZERO '0'

#define RSP_GPIO_MAX_GPIO_FNAME 64
#define RSP_MAX_GPIO 32
#define BUFFER_MAX 256

#define WAIT_SYS_TIME 100  /* Empiric wait time for SYSFS update */
/*****************************************************************************/
/*                         PRIVATE TYPES DEFINITIONS                         */
/*****************************************************************************/
/**
 * Typedef with the handle struct, also is a list
 * to make sure we can do a proper cleanup of the driver.
 */
typedef struct st_ST8DD_RPI_GPIO_Handle
{
int fd_value;
int fd_direction;
u32 GPIO_Ordinal;
struct st_ST8DD_RPI_GPIO_Handle *Next;

} ST8DD_RPI_GPIO_Handle;

/*****************************************************************************/
/*                            PRIVATE VARIABLES                              */
/*****************************************************************************/
static ST8DD_RPI_GPIO_Handle *HeadInstances = NULL;

/*****************************************************************************/
/*                       PRIVATE FUNCTIONS PROTOTYPES                        */
/*****************************************************************************/
/**
 * Function to request a GPIO to the Raspberry module
 */
static ST8DD_ErrorCodes ST8DD_RPI_RequestGPIO(ST8DD_GPIO_Id, ST8DD_GPIO_Handle *Handle);
static ST8DD_ErrorCodes ST8DD_RPI_ReleaseGPIO(ST8DD_GPIO_Handle Handle);
static ST8DD_ErrorCodes ST8DD_RPI_SetDirectionGPIO(ST8DD_GPIO_Handle Handle, ST8DD_GPIO_Direction Dir);
static ST8DD_ErrorCodes ST8DD_RPI_GetDirectionGPIO(ST8DD_GPIO_Handle Handle, ST8DD_GPIO_Direction *Dir);
static ST8DD_ErrorCodes ST8DD_RPI_SetValueGPIO(ST8DD_GPIO_Handle Handle, ST8DD_GPIO_Level Value);
static ST8DD_ErrorCodes ST8DD_RPI_GetValueGPIO(ST8DD_GPIO_Handle Handle, ST8DD_GPIO_Level *Dir);

/*****************************************************************************/
/*                         INTERFACES DECLARATIONS                           */
/*****************************************************************************/
/*
 * Creation of the Raspberry interface
 */
const ST8_GPIO_Interface ST8_RPI_GPIO_Interface =
{ ST8DD_RPI_RequestGPIO, ST8DD_RPI_ReleaseGPIO, ST8DD_RPI_SetDirectionGPIO, ST8DD_RPI_GetDirectionGPIO, ST8DD_RPI_SetValueGPIO, ST8DD_RPI_GetValueGPIO };
/*****************************************************************************/
/*                     PRIVATE FUNCTIONS IMPLEMENTATIONS                     */
/*****************************************************************************/
__attribute__ ((destructor)) static void ST8DD_ForceRelease(void)
{
    while (HeadInstances != NULL)
    {
        (void) ST8DD_RPI_ReleaseGPIO(HeadInstances);
    }
}
/**
 * Due to an ugly bug, the GPIOs files are not ready right after
 * its creation on the exports file, we have to wait a empiric
 * ammount of time
 * @param Milliseconds
 */
static void ST8DD_RPI_SleepMS(u32 Milliseconds)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = Milliseconds / 1000;
    ts.tv_nsec = (Milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(Milliseconds * 1000);
#endif
}
/**
 * This function checks if a GPIO instance already exists on the system
 * @param Id Ordinal of the GPIO to test
 * @return a file handle with the file value
 */
static int T8DD_RPI_CheckGPIO_Exists(ST8DD_GPIO_Id Id)
{
    int GPIO_Handle;
    char filename[RSP_GPIO_MAX_GPIO_FNAME];
    snprintf(filename, sizeof(filename) - 1,
    RSP_GPIO_SYSFS_PATH "/" RSP_GPIO_INSTANCE_DIR "%.d/" RSP_GPIO_VALUE_FILE, (u32) Id);
    GPIO_Handle = open(filename, O_RDWR | O_EXCL);
    return GPIO_Handle;
}
/**
 * This internal function closes the handles associated to a GPIO
 * and removes the entry from the exports file.
 * It does not perform any value validation because that is assumed
 * to be performed by the callers.
 * @param Handle
 * @return
 */
static ST8DD_ErrorCodes ST8DD_InternalReleaseGPIO(ST8DD_RPI_GPIO_Handle *Handle)
{
    int fdGPIOExport;
    char buffer[BUFFER_MAX];
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    ssize_t bytes_written;
    close(Handle->fd_value);
    close(Handle->fd_direction);
    fdGPIOExport = open(RSP_GPIO_SYSFS_PATH RSP_GPIO_UNEXPORTS_PATH, O_WRONLY);
    if (-1 == fdGPIOExport)
    {
        LogFatal("Cannot open GPIO un-exports file (%s) ", RSP_GPIO_SYSFS_PATH RSP_GPIO_EXPORTS_PATH);
        RetValue = ST8DD_Err_InvalidPlatform;
    }
    else
    {
        int TestHandle;
        bytes_written = snprintf(buffer, BUFFER_MAX, "%d", Handle->GPIO_Ordinal);
        if (bytes_written != write(fdGPIOExport, buffer, bytes_written))
        {
            LogFatal("Cannot write to GPIO exports ");
            RetValue = ST8DD_Err_InvalidPlatform;
        }
        // Now, let's check if the GPIO is really closed
        fsync(fdGPIOExport);
        ST8DD_RPI_SleepMS(WAIT_SYS_TIME);
        close(fdGPIOExport);
        TestHandle = T8DD_RPI_CheckGPIO_Exists(Handle->GPIO_Ordinal);
        if (-1 != TestHandle)
        {
            close(TestHandle);
            LogFatal("GPIO (%d) still exists ", Handle->GPIO_Ordinal);
            RetValue = ST8DD_Err_InvalidArgument;
        }

    }
    return RetValue;
}
/*****************************************************************************/
/*                    PUBLIC FUNCTIONS IMPLEMENTATIONS                       */
/*****************************************************************************/
/**
 * FUnction that requests a GPIO instance for the driver.
 * It checks that the GPIO does not exists already, creates the instance and check that it exsits already
 * @param Id
 * @param Handle
 * @return
 */
static ST8DD_ErrorCodes ST8DD_RPI_RequestGPIO(ST8DD_GPIO_Id Id, ST8DD_GPIO_Handle *Handle)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    int fdGPIOExport;
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    ST8DD_RPI_GPIO_Handle **InternalHandle;
    if (NULL == Handle)
    {
        LogFatal("Invalid handle pointer ");
        RetValue = ST8DD_Err_InvalidArgument;
    }
    else
    {
        InternalHandle = (ST8DD_RPI_GPIO_Handle **) Handle;
        if ((unsigned int) Id >= RSP_MAX_GPIO)
        {
            LogFatal("Invalid GPIO ordinal %d", Id);
            RetValue = ST8DD_Err_InvalidArgument;

        }
        else
        {
            int TestHandle;
            TestHandle = T8DD_RPI_CheckGPIO_Exists(Id);
            if (-1 != TestHandle)
            {
                close(TestHandle);
                LogFatal("GPIO (%d) already exists ", Id);
                RetValue = ST8DD_Err_InvalidArgument;

            }
            else
            {
                fdGPIOExport = open(RSP_GPIO_SYSFS_PATH RSP_GPIO_EXPORTS_PATH,
                O_WRONLY);
                if (-1 == fdGPIOExport)
                {
                    LogFatal("Cannot open GPIO exports (%s) ", RSP_GPIO_SYSFS_PATH RSP_GPIO_EXPORTS_PATH);
                    RetValue = ST8DD_Err_InvalidPlatform;
                }
                else
                {
                    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", Id);
                    if (bytes_written != write(fdGPIOExport, buffer, bytes_written))
                    {
                        LogFatal("Cannot write to GPIO exports ");
                        RetValue = ST8DD_Err_InvalidPlatform;
                    }
                    // Now, let's check if the GPIO is really opened
                    ST8DD_RPI_SleepMS(WAIT_SYS_TIME);
                    fsync(fdGPIOExport);
                    // There is a bug in the GPIO SYSFS kernel implementation, and the file
                    // is not ready exactly right after returning from the creation /deletion call

                    close(fdGPIOExport);
                    *InternalHandle = malloc(sizeof(ST8DD_RPI_GPIO_Handle));
                    if (NULL == *Handle)
                    {
                        LogFatal("Cannot allocate memory for the handle");
                        RetValue = ST8DD_Err_OutOfMemory;
                    }
                    else
                    {
                        (*InternalHandle)->fd_value = T8DD_RPI_CheckGPIO_Exists(Id);
                        if (-1 == (*InternalHandle)->fd_value)
                        {
                            LogFatal("Cannot create the GPIO %d", Id);
                            RetValue = ST8DD_Err_InvalidPlatform;
                        }
                        else
                        {
                            char filename[RSP_GPIO_MAX_GPIO_FNAME];
                            snprintf(filename, sizeof(filename) - 1,
                            RSP_GPIO_SYSFS_PATH "/" RSP_GPIO_INSTANCE_DIR "%.d/" RSP_GPIO_DIRECTION_FILE, (u32) Id);
                            (*InternalHandle)->fd_direction = open(filename,
                            O_RDWR | O_EXCL);
                            if (-1 == (*InternalHandle)->fd_direction)
                            {
                                LogFatal("Cannot create the GPIO (direction)%d", Id);
                                RetValue = ST8DD_Err_InvalidPlatform;

                            }
                            else
                            {
                                (*InternalHandle)->GPIO_Ordinal = Id;
                                (*InternalHandle)->Next = HeadInstances;
                                HeadInstances = (*InternalHandle);
                            }
                        }
                    }

                }
            }
        }
    }
    return RetValue;

}
/**
 * Function to release a GPIO and unassign it to the driver
 * @param Handle
 * @return
 */
static ST8DD_ErrorCodes ST8DD_RPI_ReleaseGPIO(ST8DD_GPIO_Handle Handle)
{
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    ST8DD_RPI_GPIO_Handle *InternalHandle = (ST8DD_RPI_GPIO_Handle *) Handle;
    if (NULL == InternalHandle)
    {
        LogFatal("Invalid handle pointer ");
        RetValue = ST8DD_Err_InvalidArgument;
    }
    else
    {
        ST8DD_RPI_GPIO_Handle *CurrentPointer = HeadInstances;
        ST8DD_InternalReleaseGPIO(Handle);
        /**
         * Let's remove the element from the list of opened ones
         */
        if (HeadInstances == InternalHandle)
        {
            HeadInstances = InternalHandle->Next;
        }
        else
        {
            do
            {
                if (InternalHandle->Next == CurrentPointer)
                {
                    CurrentPointer->Next = InternalHandle->Next;
                    break;
                }
                CurrentPointer = CurrentPointer->Next;
            } while (CurrentPointer != NULL);
        }
        if (NULL == CurrentPointer)
        {
            LogFatal("Handle not found in internal lists (%p)", Handle);
            RetValue = ST8DD_Err_InvalidArgument;
        }
        else
        {
            free(Handle);
        }
        return RetValue;
    }

    return (ST8DD_Err_NotImplemented);
}
/**
 *
 * @param Handle
 * @param Dir
 * @return
 */
static ST8DD_ErrorCodes ST8DD_RPI_SetDirectionGPIO(ST8DD_GPIO_Handle Handle, ST8DD_GPIO_Direction Dir)
{
    const char *DirectionStrPtr;
    ST8DD_RPI_GPIO_Handle *InternalHandle = (ST8DD_RPI_GPIO_Handle *) Handle;
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (NULL == Handle)
    {
        RetValue = ST8DD_Err_InvalidArgument;
        LogError("Invalid argument");
    }
    else
    {
        switch (Dir)
        {
            case ST8DD_GPIO_Sense_In:
                DirectionStrPtr = RSP_GPIO_DIRECTION_VAL_IN;
                break;
            case ST8DD_GPIO_Sense_Out:
                DirectionStrPtr = RSP_GPIO_DIRECTION_VAL_OUT;
                break;
            default:
                RetValue = ST8DD_Err_InvalidArgument;
                LogError("Invalid direction %d", Dir);
                break;
        }
        if (ST8DD_Err_NoError == RetValue)
        {
            if (write(InternalHandle->fd_direction, DirectionStrPtr, strlen(DirectionStrPtr)) != strlen(DirectionStrPtr))
            {
                RetValue = ST8DD_Err_InvalidPlatform;
                LogError("Cannot set to output pin");
            }

        }
    }
    return (RetValue);
}
/**
 *
 * @param Handle
 * @param Dir
 * @return
 */
static ST8DD_ErrorCodes ST8DD_RPI_GetDirectionGPIO(ST8DD_GPIO_Handle Handle, ST8DD_GPIO_Direction *Dir)
{
    ST8DD_RPI_GPIO_Handle *InternalHandle = (ST8DD_RPI_GPIO_Handle *) Handle;
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (NULL == Handle)
    {
        RetValue = ST8DD_Err_InvalidArgument;
        LogError("Invalid argument");
    }
    else
    {
        char ChDir[32];
        lseek(InternalHandle->fd_direction,SEEK_SET,0);
        int ReadLen=read(InternalHandle->fd_direction, ChDir, sizeof(ChDir)) ;
        if (ReadLen <=0)
        {
            RetValue = ST8DD_Err_InvalidPlatform;
            LogError("Cannot get GPIO direction");
        }
        switch (ChDir[0])
        {
            case RSP_GPIO_DIR_CH_VAL_IN:
                *Dir = ST8DD_GPIO_Sense_In;
                break;
            case RSP_GPIO_DIR_CH_VAL_OUT:
                *Dir = ST8DD_GPIO_Sense_Out;
                break;
            default:
                RetValue = ST8DD_Err_InvalidPlatform;
                LogError("Invalid direction for pin (%s)", ChDir);

        }
    }

    return RetValue;
}
/**
 *
 * @param Handle
 * @param Dir
 * @return
 */
static ST8DD_ErrorCodes ST8DD_RPI_SetValueGPIO(ST8DD_GPIO_Handle Handle, ST8DD_GPIO_Level Value)
{
    const char *DirectionStrPtr;
    ST8DD_RPI_GPIO_Handle *InternalHandle = (ST8DD_RPI_GPIO_Handle *) Handle;
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (NULL == Handle)
    {
        RetValue = ST8DD_Err_InvalidArgument;
        LogError("Invalid argument");
    }
    else
    {
        switch (Value)
        {
            case ST8DD_GPIO_Level_Low:
                DirectionStrPtr = RSP_GPIO_VALUE_VAL_ZERO;
                break;
            case ST8DD_GPIO_Level_High:
                DirectionStrPtr = RSP_GPIO_VALUE_VAL_ONE;
                break;
            default:
                RetValue = ST8DD_Err_InvalidArgument;
                LogError("Invalid value for GPIO %d", Value);
                break;
        }
        if (ST8DD_Err_NoError == RetValue)
        {
            if (write(InternalHandle->fd_value, DirectionStrPtr, strlen(DirectionStrPtr)) != strlen(DirectionStrPtr))
            {
                RetValue = ST8DD_Err_InvalidPlatform;
                LogError("Cannot set to output pin");
            }

        }
    }
    return (RetValue);
}
/**
 * Reads the current value of a GPIO port
 * @param Handle
 * @param Value
 * @return
 */
static ST8DD_ErrorCodes ST8DD_RPI_GetValueGPIO(ST8DD_GPIO_Handle Handle, ST8DD_GPIO_Level *Value)
{
    ST8DD_RPI_GPIO_Handle *InternalHandle = (ST8DD_RPI_GPIO_Handle *) Handle;
    ST8DD_ErrorCodes RetValue = ST8DD_Err_NoError;
    if (NULL == Handle)
    {
        RetValue = ST8DD_Err_InvalidArgument;
        LogError("Invalid argument");
    }
    else
    {
        char ValueStr[32];
        lseek(InternalHandle->fd_value,SEEK_SET,0);
        int ReadLen=read(InternalHandle->fd_value, &ValueStr, sizeof(ValueStr)) ;
        if (ReadLen <=0)
        {
            RetValue = ST8DD_Err_InvalidPlatform;
            LogError("Cannot set to output pin");
        }
        switch (ValueStr[0])
        {
            case RSP_GPIO_VALUE_CH_VAL_ONE:
                *Value = ST8DD_GPIO_Level_High;
                break;
            case RSP_GPIO_VALUE_CH_VAL_ZERO:
                *Value = ST8DD_GPIO_Level_Low;
                break;
            default:
                RetValue = ST8DD_Err_InvalidPlatform;
                LogError("Invalid value on pin (%s)", ValueStr);

        }
    }

    return RetValue;
}

#ifdef __cplusplus
}
#endif
/*****************************************************************************/
/*                                   END                                     */
/*****************************************************************************/
