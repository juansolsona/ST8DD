/*
 * ST8DD_UserSpaceRPI_Log.c
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

    This module implements logging of the ST8 driver for Raspberry PI user space
 */
#include <stdio.h>
#include "ST8DD_Types.h"
#include "ST8DD_Log.h"


void ST8DD_RawLog(const char *str)
{
	puts(str);
}

