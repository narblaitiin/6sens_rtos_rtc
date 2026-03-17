/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================

#include "app_ds3231.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/counter.h>   
#include <zephyr/drivers/i2c.h>    
#include <zephyr/device.h>            
#include <zephyr/devicetree.h>        
#include <zephyr/posix/time.h>         
#include <zephyr/sys/timeutil.h>

//  ========== defines =====================================================================
#define STACK_SIZE 2048
#define PRIORITY   2

//  ========== globals =====================================================================
K_SEM_DEFINE(init_done_sem, 0, 1);

//  ========== interrupt sub-routine =======================================================
void rtc_thread_func(void)
{
	k_sem_take(&init_done_sem, K_FOREVER);
    const struct device *ds3231_dev = DEVICE_DT_GET_ONE(maxim_ds3231);

    while (true) {
        app_ds3231_periodic_sync(ds3231_dev);   // re-anchor offset
        k_sleep(K_SECONDS(30));
    }	
}
K_THREAD_DEFINE(rtc_thread_id, STACK_SIZE, rtc_thread_func, NULL, NULL, NULL, PRIORITY, 0, 0);

void rtc_work_handler(struct k_work *work_rtc)
{
	uint64_t ts = app_get_timestamp();
    printk("timestamp (ms): %llu\n", ts);
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
	// init and set DS3231 once at boot
    const struct device *ds3231_dev = app_ds3231_init();
    if (!ds3231_dev) {
        printk("failed to initialize DS3231\n");
        return 0;
    }
    app_ds3231_set_time(ds3231_dev, 1773741600);

    // start nRF internal RTC counter
    const struct device *nrf_rtc = DEVICE_DT_GET(DT_NODELABEL(rtc2));
    counter_start(nrf_rtc);

    printk("RTC system ready\n");

    k_sem_give(&init_done_sem);
    k_timer_start(&rtc_timer, K_SECONDS(5), K_SECONDS(10));
    return 0;
}