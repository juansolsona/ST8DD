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
       Copyright juansolsona 2016 10:04:57
    ------------------------------------------------------------------------- */
/******************************************************************************
* This file implements a basic logging mechanism interface to be refined
* by the log sync
******************************************************************************/

/*****************************************************************************/
/*                                 INCLUDES                                  */
/*****************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include "ST8DD_Types.h"
#include "ST8DD_Log.h"

/*****************************************************************************/
/*                         PRIVATE MACROS DEFINITONS                         */
/*****************************************************************************/
#define MAX_LOG_MESSAGE 256


/*****************************************************************************/
/*                         PRIVATE TYPES DEFINITIONS                         */
/*****************************************************************************/
/*****************************************************************************/
/*                            PRIVATE VARIABLES                              */
/*****************************************************************************/
static const char *ST8DD_LogLevelStr[]=
{
		"Info",
		"Warn",
		"Error",
		"Fatal",
};


/*****************************************************************************/
/*                       PRIVATE FUNCTIONS PROTOTYPES                        */
/*****************************************************************************/
/*****************************************************************************/
/*                     PRIVATE FUNCTIONS IMPLEMENTATIONS                     */
/*****************************************************************************/
/*****************************************************************************/
/*                    PUBLIC FUNCTIONS IMPLEMENTATIONS                       */
/*****************************************************************************/
/**
 *
 * @param X
 * @return
 */
const char *ST8DD_GetLogLevelStr(ST8DD_LogLevel X)
{
	return ((X > sizeof(ST8DD_LogLevelStr)/ sizeof(ST8DD_LogLevelStr[0])? "?????" : ST8DD_LogLevelStr[X]));
}


void ST8DD_ComposeLog(const char *format,...)
{
   va_list va_ar;
   char Message[MAX_LOG_MESSAGE];
   va_start(va_ar,format);
   vsnprintf(Message,sizeof(Message),format,va_ar);
   ST8DD_RawLog(Message);
}


/*****************************************************************************/
/*                                   END                                     */
/*****************************************************************************/
