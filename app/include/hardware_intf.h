#ifndef HARDWARE_INTF_H
#define HARDWARE_INTF_H

const struct device * const get_imu_device(void);
const struct device * const get_lidar_device(void);
void app_hardware_init(void);

#endif // HARDWARE_INTF_H
