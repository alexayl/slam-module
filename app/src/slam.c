#include <stdbool.h>
#include <stdio.h>
#include <zephyr/kernel.h>

#include "imu.h"
#include "slam.h"
#include "Fusion.h"

#define ROLL_PITCH_YAW 0
#define POSITION 1
#define OUTPUT_MODE ROLL_PITCH_YAW // Change this to POSITION for position output

void slam_thread(void *arg1, void *arg2, void *arg3) {

    FusionAhrs ahrs;
    FusionAhrsInitialise(&ahrs);

    static int counter = 0;
    
    imu_data_t current_imu_data;

    while (true) { 

        #if OUTPUT_MODE == ROLL_PITCH_YAW
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

            const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));

            if ((counter %= 40) == 0) { // Print every 10 iterations
                printk("Roll %0.1f, Pitch %0.1f, Yaw %0.1f\n", 
                   (double)euler.angle.roll, 
                   (double)euler.angle.pitch, 
                   (double)euler.angle.yaw);
            }
            counter++;
        }
        #elif OUTPUT_MODE == POSITION
        
        #endif

        k_sleep(K_MSEC(SAMPLE_PERIOD_MSEC)); // Check for new data every 10ms
    }
    return;
}