/*
    This file is part of ST8DD_User.

    Foobar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ST8DD_User.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INC_ST8DD_LOG_H_
#define INC_ST8DD_LOG_H_

/*****************************************************************************/
/*                                 INCLUDES                                  */
/*****************************************************************************/



#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/*                         PUBLIC MACROS DEFINITONS                          */
/*****************************************************************************/
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define FILELINE(X) (__FILE__ "::" TOSTRING(__LINE__))
#define FILENAME_LEN 48
#define LOG_FORMAT "%-6s:%-" TOSTRING(FILENAME_LEN) "." TOSTRING(FILENAME_LEN) "s:"
#define LogInfo(format,args...)  ST8DD_ComposeLog(LOG_FORMAT format,ST8DD_GetLogLevelStr(LogLevelInfo) ,FILELINE(__LINE__), ##args)
#define LogWarn(format,args...)  ST8DD_ComposeLog(LOG_FORMAT format,ST8DD_GetLogLevelStr(LogLevelWarn) ,FILELINE(__LINE__), ##args)
#define LogError(format,args...) ST8DD_ComposeLog(LOG_FORMAT format,ST8DD_GetLogLevelStr(LogLevelError),FILELINE(__LINE__), ##args)
#define LogFatal(format,args...) ST8DD_ComposeLog(LOG_FORMAT format,ST8DD_GetLogLevelStr(LogLevelFatal),FILELINE(__LINE__), ##args)


/*****************************************************************************/
/*                         PUBLIC TYPES DEFINITIONS                          */
/*****************************************************************************/
typedef enum enST8DD_LogLevel
{
	LogLevelInfo=0,
	LogLevelWarn=1,
	LogLevelError=2,
	LogLevelFatal=3
} ST8DD_LogLevel;

/*****************************************************************************/
/*                             PUBLIC VARIABLES                              */
/*****************************************************************************/
extern const char *enST8DD_LogLevelStr[];
/*****************************************************************************/
/*                        PUBLIC FUNCTIONS PROTOTYPES                        */
/*****************************************************************************/


void ST8DD_RawLog(const char *format);
void ST8DD_ComposeLog(const char *format,...);
const char *ST8DD_GetLogLevelStr(ST8DD_LogLevel );
#ifdef __cplusplus
}
#endif
#endif /* INC_ST8DD_LOG_H_ */
