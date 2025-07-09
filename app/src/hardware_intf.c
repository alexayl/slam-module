#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdlib.h>

#include "hardware_intf.h"

// local variables
static const struct device *devices[] = {
        DEVICE_DT_GET(DT_ALIAS(imu)),
        DEVICE_DT_GET(DT_ALIAS(lidar))
    };

static const enum {
    IMU,
    LIDAR
} devices_indices;

// Public functions
const struct device * const get_imu_device(void) {
    return devices[IMU];
}

const struct device * const get_lidar_device(void) {
    return devices[LIDAR];
}

void app_hardware_init(void) {

    for (size_t i = 0; i < ARRAY_SIZE(devices); i++) {
        if (!device_is_ready(devices[i])) {
            printk("Device %s is not ready\n", devices[i]->name);
            exit(EXIT_FAILURE);
        }
    }

    printk("All devices are ready\n");
    return;
}





