# ST8DD

A debug interface for STM 8 microcontrollers

This project is the implementation of a debug driver for the ST8 family of microcontrollers. It is independent of the driver used to handle the GPIO pins. 

The final goal is to implement a SYSFS driver with a pointer for read/write of data and code sections and a status pointer for the 
