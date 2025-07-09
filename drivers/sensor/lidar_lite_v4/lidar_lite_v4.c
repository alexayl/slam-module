/*
 * Copyright (c) 2025 Alex
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT garmin_lidar_lite_v4

#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>

#include "lidar_lite_v4.h"

int lidar_lite_v4_init(const struct device *dev) {

    const struct lidar_lite_v4_config *config = dev->config;

    if (!device_is_ready(config->i2c.bus)) {
        return -ENODEV;
    }

    return 0;
}

static int lidar_lite_v4_sample_fetch(const struct device *dev, enum sensor_channel chan) {
    struct lidar_lite_v4_data *data = dev->data;
    const struct lidar_lite_v4_config *config = dev->config;
    uint8_t status;
    uint8_t distance_bytes[2];
    int ret;

    if (!device_is_ready(config->i2c.bus)) {
        return -ENODEV;
    }

    // Write command to trigger measurement
    ret = i2c_reg_write_byte_dt(&config->i2c, LIDAR_LITE_V4_REG_MEASURE, LIDAR_LITE_V4_CMD_MEASURE);
    if (ret < 0) {
        return ret;
    }

    // Read status register and wait until measurement completes
    int timeout = MAX_TIMEOUT_MS;
    do {
        ret = i2c_reg_read_byte_dt(&config->i2c, LIDAR_LITE_V4_REG_STATUS, &status);
        if (ret < 0) {
            return ret;
        }
        k_msleep(1);
        timeout--;
    } while ((status & LIDAR_LITE_V4_STATUS_BUSY) && timeout > 0);
    if (timeout <= 0) {
        return -ETIMEDOUT;
    }

    // Read distance data (high byte then low byte)
    ret = i2c_reg_read_byte_dt(&config->i2c, LIDAR_LITE_V4_REG_DISTANCE_HIGH, &distance_bytes[0]);
    if (ret < 0) {
        return ret;
    }
    
    ret = i2c_reg_read_byte_dt(&config->i2c, LIDAR_LITE_V4_REG_DISTANCE_LOW, &distance_bytes[1]);
    if (ret < 0) {
        return ret;
    }

    // Combine high and low bytes to get 16-bit distance in centimeters
    data->distance = (distance_bytes[0] << 8) | distance_bytes[1];

    return 0;
}

static int lidar_lite_v4_channel_get(const struct device *dev,
                    enum sensor_channel chan,
                    struct sensor_value *val) {

    struct lidar_lite_v4_data *data = dev->data;

    switch (chan) {
        case SENSOR_CHAN_DISTANCE:
        case SENSOR_CHAN_ALL:
            val->val1 = data->distance; // Distance in cm
            val->val2 = 0; // No fractional part
            break;
        default:
            return -ENOTSUP;
    }
    return 0;
}

static const struct sensor_driver_api lidar_lite_v4_driver_api = {
	.sample_fetch = lidar_lite_v4_sample_fetch,
	.channel_get = lidar_lite_v4_channel_get,
};


#define LIDAR_LITE_V4_DEFINE(inst) \
    static struct lidar_lite_v4_data lidar_lite_v4_data_##inst; \
    static const struct lidar_lite_v4_config lidar_lite_v4_config_##inst = { \
        .i2c = I2C_DT_SPEC_INST_GET(inst), \
    }; \
    SENSOR_DEVICE_DT_INST_DEFINE(inst, \
        lidar_lite_v4_init, \
        NULL, \
        &lidar_lite_v4_data_##inst, \
        &lidar_lite_v4_config_##inst, \
        POST_KERNEL, \
        CONFIG_SENSOR_INIT_PRIORITY, \
        &lidar_lite_v4_driver_api);

DT_INST_FOREACH_STATUS_OKAY(LIDAR_LITE_V4_DEFINE)