/*
 * gpio_hal.h
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

#ifndef INC_GPIO_HAL_H_
#define INC_GPIO_HAL_H_

#include "ST8DD_Error.h"
/**
 * Enumeration with the possible values of a GPIO port
 */
typedef enum enST8DD_GPIO_Level
{
	ST8DD_GPIO_Level_Low=0,
	ST8DD_GPIO_Level_High=1,

} ST8DD_GPIO_Level;
/**
 * Direction of the GPIO pin
 */
typedef enum enST8DD_GPIO_Direction
{
	ST8DD_GPIO_Sense_In=1,
	ST8DD_GPIO_Sense_Out=2

} ST8DD_GPIO_Direction;
typedef unsigned char ST8DD_GPIO_Id;

/**
 * Abtract structure with a handle to be used internally
 * by the GPIO Driver
 */
typedef void * ST8DD_GPIO_Handle;
/**
 * Structure with the interface to be implemented
 * by a hardware driver
 */
typedef struct stST8_GPIO_Interface
{
	ST8DD_ErrorCodes (*RequestGPIO)(ST8DD_GPIO_Id, ST8DD_GPIO_Handle *Handle);
	ST8DD_ErrorCodes (*ReleaseGPIO)(ST8DD_GPIO_Handle Handle);
	ST8DD_ErrorCodes (*SetDirectionGPIO)(ST8DD_GPIO_Handle Handle,ST8DD_GPIO_Direction Dir);
	ST8DD_ErrorCodes (*GetDirectionGPIO)(ST8DD_GPIO_Handle Handle,ST8DD_GPIO_Direction *Dir);
	ST8DD_ErrorCodes (*SetValueGPIO)(ST8DD_GPIO_Handle Handle,ST8DD_GPIO_Level Dir);
	ST8DD_ErrorCodes (*GetValueGPIO)(ST8DD_GPIO_Handle Handle,ST8DD_GPIO_Level *Dir);
} ST8_GPIO_Interface;


#endif /* INC_GPIO_HAL_H_ */
