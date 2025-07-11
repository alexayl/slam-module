#include <stdbool.h>
#include <stdio.h>
#include <zephyr/kernel.h>

#include "imu.h"
#include "slam.h"
#include "Fusion.h"

// #define DEBUG_SLAM

slam_output_t slam_output;
K_MSGQ_DEFINE(positioning_data, sizeof(slam_output_t), 10, 4);

void slam_thread(void *arg1, void *arg2, void *arg3) {

    FusionAhrs ahrs;
    FusionAhrsInitialise(&ahrs);

    #ifdef DEBUG_SLAM
    printk("SLAM thread started\n");
    static int counter = 0;
    #endif
    
    imu_data_t current_imu_data;

    while (true) { 

        // Get the latest IMU data
        if (get_latest_imu_data(&current_imu_data)) {
            // Convert sensor_value to float for Fusion library
            const FusionVector gyroscope = {{
                sensor_value_to_double(&current_imu_data.gyro[0]),
                sensor_value_to_double(&current_imu_data.gyro[1]),
                sensor_value_to_double(&current_imu_data.gyro[2])
            }};
            
            const FusionVector accelerometer = {{
                sensor_value_to_double(&current_imu_data.accel[0]),
                sensor_value_to_double(&current_imu_data.accel[1]),
                sensor_value_to_double(&current_imu_data.accel[2])
            }};

            FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, SAMPLE_PERIOD_SEC);
            const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);
            const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
            
            #ifdef DEBUG_SLAM
            if ((counter %= 40) == 0) { // Print every 40 iterations
                // printk("Roll %0.1f, Pitch %0.1f, Yaw %0.1f\n", euler.angle.roll, euler.angle.pitch, euler.angle.yaw);
                printk("X %0.1f, Y %0.1f, Z %0.1f\n", earth.axis.x, earth.axis.y, earth.axis.z);
            }
            counter++;
            #endif
        
        // Set the output data
        slam_output.roll = euler.angle.roll;
        slam_output.pitch = euler.angle.pitch;
        slam_output.yaw = euler.angle.yaw;
        slam_output.earth_x = earth.axis.x;
        slam_output.earth_y = earth.axis.y;
        slam_output.earth_z = earth.axis.z;
        slam_output.timestamp = k_uptime_get();
        
        k_msgq_put(&positioning_data, &slam_output, K_NO_WAIT);
        }

        k_sleep(K_MSEC(SAMPLE_PERIOD_MSEC)); // Check for new data every 10ms
    }
    return;
}