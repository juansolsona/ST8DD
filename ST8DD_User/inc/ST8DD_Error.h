/*
 * STL8_Error.h
 *
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

#ifndef INC_STL8_ERROR_H_
#define INC_STL8_ERROR_H_

/**
 * Enumeration with common error codes for ST8_DebugDriver
 */

typedef enum enST8DD_ErrorCodes
{
	ST8DD_Err_NoError=0,
	ST8DD_Err_InvalidArgument=-1,
	ST8DD_Err_InvalidPlatform=-2,
	ST8DD_Err_NotImplemented=-3,
	ST8DD_Err_OutOfMemory=-4,

} ST8DD_ErrorCodes;

#endif /* INC_STL8_ERROR_H_ */
