#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

#include "hardware_intf.h"
#include "imu.h"

#define DEBUG_IMU


void imu_thread(void *arg1, void *arg2, void *arg3) {

    const struct device *dev = get_imu_device();
    struct sensor_value temperature;
	struct sensor_value accel[3];
	struct sensor_value gyro[3];

    int rc;

    while (1) {
        rc = sensor_sample_fetch(dev);
        if (rc == 0) {
            rc = sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ,
                        accel);
        }
        if (rc == 0) {
            rc = sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ,
                        gyro);
        }
        if (rc == 0) {
            rc = sensor_channel_get(dev, SENSOR_CHAN_DIE_TEMP,
                        &temperature);
        }
        if (rc == 0) {
            #ifdef DEBUG_IMU
            printk("%g Cel\n"
                "  accel %f %f %f m/s/s\n"
                "  gyro  %f %f %f rad/s\n",
                sensor_value_to_double(&temperature),
                sensor_value_to_double(&accel[0]),
                sensor_value_to_double(&accel[1]),
                sensor_value_to_double(&accel[2]),
                sensor_value_to_double(&gyro[0]),
                sensor_value_to_double(&gyro[1]),
                sensor_value_to_double(&gyro[2]));
            #endif
        } else {
            #ifdef DEBUG_IMU
            printk("sample fetch/get failed: %d\n", rc);
            #endif
        }

        k_sleep(K_MSEC(100)); // Sleep for 100 milliseconds

    }

    return;
}