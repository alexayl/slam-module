#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdlib.h>

#include "imu.h"
#include "lidar.h"
#include "slam.h"
#include "wifi_comm.h"
#include "hardware_intf.h"

#define STACK_SIZE      2048

#define IMU_PRIORITY     3
#define LIDAR_PRIORITY   3
#define SLAM_PRIORITY    2
#define WIFI_PRIORITY    1


K_THREAD_STACK_DEFINE(imu_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(lidar_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(slam_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(wifi_stack, STACK_SIZE);

static struct k_thread imu_thread_data;
static struct k_thread lidar_thread_data;
static struct k_thread slam_thread_data;
static struct k_thread wifi_thread_data;

typedef bool (*init_func_t)(void);

int main(void) {

    app_hardware_init();

    k_thread_create(&imu_thread_data, imu_stack, STACK_SIZE,
                    imu_thread, NULL, NULL, NULL,
                    IMU_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&lidar_thread_data, lidar_stack, STACK_SIZE,
                    lidar_thread, NULL, NULL, NULL,
                    LIDAR_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&slam_thread_data, slam_stack, STACK_SIZE,
                    slam_thread, NULL, NULL, NULL,
                    SLAM_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&wifi_thread_data, wifi_stack, STACK_SIZE,
                    wifi_thread, NULL, NULL, NULL,
                    WIFI_PRIORITY, 0, K_NO_WAIT);

    return EXIT_SUCCESS;
}
