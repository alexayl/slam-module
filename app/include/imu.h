#ifndef IMU_H
#define IMU_H

#include <zephyr/drivers/sensor.h>
#include <stdbool.h>

#define SAMPLE_PERIOD_MSEC (10)
#define SAMPLE_PERIOD_SEC (SAMPLE_PERIOD_MSEC / 1000.0f)
#define SAMPLE_RATE_HZ (1000.0f / SAMPLE_PERIOD_MSEC)

typedef struct {
    struct sensor_value accel[3];
    struct sensor_value gyro[3];
    struct sensor_value temperature;
    bool data_ready;
} imu_data_t;

// Function declarations
void imu_thread(void *, void *, void *);
bool get_latest_imu_data(imu_data_t *data);

#endif // IMU_H