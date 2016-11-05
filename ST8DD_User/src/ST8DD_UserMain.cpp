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


/*****************************************************************************/
/*                    PUBLIC FUNCTIONS IMPLEMENTATIONS                       */
/*****************************************************************************/
int main(int argc,char **argv)
{
   extern int run_tests(int argc, char **argv);
	LogInfo("App start");
	return run_tests(argc, argv);
	return EXIT_SUCCESS;
}
/*****************************************************************************/
/*                                   END                                     */
/*****************************************************************************/
