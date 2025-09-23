/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_RTC_H
#define APP_RTC_H

//  ========== includes ====================================================================
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/sys_clock.h> 

//  ========== defines =====================================================================
#define ONE_YEAR_MS                 365LL * 24 * 60 * 60 * 1000
#define CONFIG_COUNTER_NRF_RTC

//  ========== prototypes ==================================================================
const struct device *app_rtc_init(void);
int8_t app_rtc_set_time(const struct device *rtc_dev, uint64_t target_time_ms);
int8_t  app_rtc_sync_uptime(const struct device *rtc_dev);
uint64_t app_rtc_get_time();
int8_t app_rtc_periodic_sync(const struct device *rtc_dev);

#endif /* APP_RTC_H */