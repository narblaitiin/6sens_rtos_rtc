# Code for 6Sens Project : testing two solutions to have a timestamp to the millisecond (internal RTC of MDBT50Q and external DS3231 RTC device)

## Overview
This application contains example code to allow testing a precise timestamp .
This code will allow you to synchronize an event on the different sensors (precision to 100ms) and thus obtain co-detection of a possible event.

The version of Zephyr RTOS used is the version v4.0.0.

## Board used
Original MDBT50Q board, powered by battery/solar panel. (see 6sens_prj repository/hardware part, for more information.)

## Building and Running
The following commands clean build folder, build and flash the sample:

**Command to use**
````
west build -t pristine

west build -p always -b mdbt50q_lora_dev applications/6sens_rtos_rtc

west flash --runner jlink