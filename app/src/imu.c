#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

#include "hardware_intf.h"
#include "imu.h"

// Shared IMU data structure
imu_data_t shared_imu_data = {0};
K_MUTEX_DEFINE(imu_data_mutex);

bool get_latest_imu_data(imu_data_t *data) {
    if (k_mutex_lock(&imu_data_mutex, K_MSEC(10)) == 0) {
        if (shared_imu_data.data_ready) {
            *data = shared_imu_data;
            k_mutex_unlock(&imu_data_mutex);
            return true;
        }
        k_mutex_unlock(&imu_data_mutex);
    }
    return false;
}

// #define DEBUG_IMU


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
            // Update shared data structure
            if (k_mutex_lock(&imu_data_mutex, K_MSEC(1)) == 0) {
                // Copy new data to shared structure
                for (int i = 0; i < 3; i++) {
                    shared_imu_data.accel[i] = accel[i];
                    shared_imu_data.gyro[i] = gyro[i];
                }
                shared_imu_data.temperature = temperature;
                shared_imu_data.data_ready = true;
                k_mutex_unlock(&imu_data_mutex);
            }
            

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
        }
        #ifdef DEBUG_IMU
        else {
            printk("sample fetch/get failed: %d\n", rc);
        }
        #endif

        k_sleep(K_MSEC(SAMPLE_PERIOD_MSEC)); // Sleep for 10 milliseconds

    }

    return;
}