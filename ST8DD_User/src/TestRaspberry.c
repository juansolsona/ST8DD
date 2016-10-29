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
       $File name:  $
    -------------------------------------------------------------------------
    -------------------------------------------------------------------------
    Info:
       Copyright juansolsona 2016 10:04:57
    ------------------------------------------------------------------------- */
/******************************************************************************
* This file implements .....
******************************************************************************/

/*****************************************************************************/
/*                                 INCLUDES                                  */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "ST8DD_GPIO_HAL.h"
#include "ST8DD_Types.h"
#include "ST8DD_Log.h"
/*****************************************************************************/
/*                         PRIVATE MACROS DEFINITONS                         */
/*****************************************************************************/
/*****************************************************************************/
/*                         PRIVATE TYPES DEFINITIONS                         */
/*****************************************************************************/
/*****************************************************************************/
/*                            PRIVATE VARIABLES                              */
/*****************************************************************************/
/*****************************************************************************/
/*                       PRIVATE FUNCTIONS PROTOTYPES                        */
/*****************************************************************************/
/*****************************************************************************/
/*                     PRIVATE FUNCTIONS IMPLEMENTATIONS                     */
/*****************************************************************************/


int TestGPIO(void)
{
   extern ST8_GPIO_Interface ST8_RPI_GPIO_Interface;
   ST8DD_ErrorCodes ErrCode;
   ST8DD_GPIO_Handle Handle;
   ST8DD_GPIO_Handle Handle2;
// LogLevelInfo("Enter " __func__);
   ErrCode = ST8_RPI_GPIO_Interface.RequestGPIO(1, &Handle);
   if (ST8DD_Err_NoError != ErrCode)
   {
      LogError("Error testing GPIO %d", ErrCode);
   }
   else
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

   return 0;
}
/*****************************************************************************/
/*                    PUBLIC FUNCTIONS IMPLEMENTATIONS                       */
/*****************************************************************************/
int main(void)
{
	LogInfo("App start");
	TestGPIO();
	return EXIT_SUCCESS;
}
/*****************************************************************************/
/*                                   END                                     */
/*****************************************************************************/
