#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdlib.h>

#include "hardware_intf.h"

// local variables
static const struct device *devs[] = {
        DEVICE_DT_GET(DT_ALIAS(imu)),
        DEVICE_DT_GET(DT_ALIAS(lidar))
    };

void app_hardware_init(void) {

    for (size_t i = 0; i < ARRAY_SIZE(devs); i++) {
        if (!device_is_ready(devs[i])) {
            printk("Device %s is not ready\n", devs[i]->name);
            exit(EXIT_FAILURE);
        }
    }
    
}

