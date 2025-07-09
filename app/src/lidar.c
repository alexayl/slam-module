#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

#include "hardware_intf.h"
#include "lidar.h"

#define DEBUG_LIDAR

void lidar_thread(void *arg1, void *arg2, void *arg3) {

    const struct device *dev = get_lidar_device();
    struct sensor_value distance;

    int rc;

    while (1) {
        rc = sensor_sample_fetch(dev);

        if (rc == 0) {
            rc = sensor_channel_get(dev, SENSOR_CHAN_DISTANCE,
                        &distance);
        }
        if (rc == 0) {
            #ifdef DEBUG_LIDAR
            printk("Distance: %g m\n", sensor_value_to_double(&distance));
            #endif
        } else {
            #ifdef DEBUG_LIDAR
            printk("sample fetch/get failed: %d\n", rc);
            #endif
        }

        k_sleep(K_MSEC(100)); // Sleep for 100 milliseconds

    }

    return;
}