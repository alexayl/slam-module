#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/net/socket.h>

#include "wifi_comm.h"
#include "slam.h"

#define MAX_BATCH 16

static slam_output_t slam_output[MAX_BATCH];
static int message_count;

void wifi_thread(void *arg1, void *arg2, void *arg3) {

    while (1) {
        // Drain the message queue
        message_count = 0;
        while (message_count < MAX_BATCH && k_msgq_get(&positioning_data, &slam_output[message_count], K_NO_WAIT) == 0) {
            message_count++;
        }

        // Send the packets over Wi-Fi
        if (message_count > 0) {
            continue;
        }
    }

    return;
}
