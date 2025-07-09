/*
 * Copyright (c) 2025 Alex Aylward
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_LIDAR_LITE_V4_H_
#define ZEPHYR_DRIVERS_SENSOR_LIDAR_LITE_V4_H_

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>

#define MAX_TIMEOUT_MS 100

#define LIDAR_LITE_V4_REG_MEASURE           0x00
#define LIDAR_LITE_V4_REG_STATUS            0x01
#define LIDAR_LITE_V4_REG_DISTANCE_HIGH     0x0f
#define LIDAR_LITE_V4_REG_DISTANCE_LOW      0x10

#define LIDAR_LITE_V4_CMD_MEASURE           0x04
#define LIDAR_LITE_V4_STATUS_BUSY           BIT(0)

struct lidar_lite_v4_config {
    struct i2c_dt_spec i2c;
};

struct lidar_lite_v4_data {
    uint16_t distance;
};

#endif /* ZEPHYR_DRIVERS_SENSOR_LIDAR_LITE_V4_H_ */