#ifndef SLAM_H
#define SLAM_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
    float roll, pitch, yaw;
    float earth_x, earth_y, earth_z;
    uint32_t timestamp;
} slam_output_t;

extern struct k_msgq positioning_data;

void slam_thread(void *, void *, void *);
#endif // SLAM_H
