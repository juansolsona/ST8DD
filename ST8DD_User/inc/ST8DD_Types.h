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

#ifndef INC_STL8_TYPES_H_
#define INC_STL8_TYPES_H_
/*****************************************************************************/
/*                                 INCLUDES                                  */
/*****************************************************************************/

/*****************************************************************************/
/*                         PUBLIC MACROS DEFINITONS                          */
/*****************************************************************************/
#define FALSE 0
#define TRUE (~FALSE)
/*****************************************************************************/
/*                         PUBLIC TYPES DEFINITIONS                          */
/*****************************************************************************/

typedef unsigned long long u64;
typedef signed long long s64;

typedef unsigned int u32;
typedef signed int i32;

typedef unsigned short u16;
typedef signed short s16;

typedef unsigned char u8;
typedef signed char s8;

typedef u8 bool;

/*****************************************************************************/

#endif /* INC_STL8_TYPES_H_ */
