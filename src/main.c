/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_rtc.h"
#include "app_ds3231.h"

#include <zephyr/kernel.h>
#include <stdbool.h>
#include <stdio.h>

//  ========== defines =====================================================================
#define STACK_SIZE 2048
#define PRIORITY   2

//  ========== interrupt sub-routine =======================================================
bool rtc_thread_flag = true;
void rtc_thread_func(void)
{
	printk("periodic sync thread started\n");

	//const struct device *rtc_dev = DEVICE_DT_GET(DT_NODELABEL(rtc0));
	const struct device *ds3231_dev = DEVICE_DT_GET_ONE(maxim_ds3231);
	while (rtc_thread_flag == true) {
        printk("performing periodic action\n");
    //	(void)app_rtc_periodic_sync(rtc_dev);
		(void)app_ds3231_periodic_sync(ds3231_dev);
        k_sleep(K_SECONDS(30));		
	}
}
K_THREAD_DEFINE(rtc_thread_id, STACK_SIZE, rtc_thread_func, NULL, NULL, NULL, PRIORITY, 0, 0);

void rtc_work_handler(struct k_work *work_rtc)
{
	//printk("test internal and DS3231 RTC device\n");

	// uint64_t timestamp_rtc =  app_rtc_get_time();
	// printk("timestamp in ms (RTC): %llu\n", timestamp_rtc);

	uint64_t timestamp_ds3231 = app_ds3231_get_time();
	printk("timestamp in ms (DS3231): %llu\n", timestamp_ds3231);
}
K_WORK_DEFINE(rtc_work, rtc_work_handler);

void rtc_timer_handler(struct k_timer *rtc_dum)
{
	k_work_submit(&rtc_work);
}
K_TIMER_DEFINE(rtc_timer, rtc_timer_handler, NULL);

// ========== main =========================================================================
int8_t main(void)
{
	// initialize DS3231 RTC device via I2C (Pins: SDA -> P0.09, SCL -> P0.0)
	const struct device *ds3231_dev = app_ds3231_init();
    if (!ds3231_dev) {
        printk("failed to initialize RTC device\n");
        return 0;
    } else {
		app_ds3231_set_time(ds3231_dev, 1770372264); // set to "2024-07-19 12:00:00" UTC
	}

	// initialize on-board RTC of MDBT50Q
	// const struct device *rtc_dev = app_rtc_init();
    // if (!rtc_dev) {
    //     printk("failed to initialize RTC device\n");
    //     return 0;
    // } else {
	// 	app_rtc_set_time(rtc_dev, 1721050200000ULL); // e.g., for "2024-07-15 12:30:00 UTC" in ms
	// }

	printk("RTC DS3231 and internal RTC Example\n");

	// enable periodic rtc sync thread
	rtc_thread_flag = true;

	// start the timer to trigger the interrupt subroutine every 30 seconds
	k_timer_start(&rtc_timer, K_NO_WAIT, K_MSEC(5000));
	return 0;
}