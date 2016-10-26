/*
 * ST8DD_Log.c
 *
 *  Created on: 23 oct. 2016
 *      Author: juansolsona

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdarg.h>
#include <stdio.h>
#include "ST8DD_Types.h"
#include "ST8DD_Log.h"

#define MAX_LOG_MESSAGE 256


static const char *ST8DD_LogLevelStr[]=
{
		"Info",
		"Warn",
		"Error",
		"Fatal",
};


void ST8DD_ComposeLog(const char *format,...)
{
	va_list va_ar;
	char Message[MAX_LOG_MESSAGE];
	va_start(va_ar,format);
	vsnprintf(Message,sizeof(Message),format,va_ar);
	ST8DD_RawLog(Message);
}


const char *ST8DD_GetLogLevelStr(ST8DD_LogLevel X)
{
	return ((X > sizeof(ST8DD_LogLevelStr)/ sizeof(ST8DD_LogLevelStr[0])? "?????" : ST8DD_LogLevelStr[X]));
}
