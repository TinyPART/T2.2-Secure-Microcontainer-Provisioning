#include <stdint.h>
#include "bpf/bpfapi/helpers.h"

#define HRS_SENSE_KEY_DIRECTION     10
#define HRS_SENSE_KEY_VALUE         11

#define HRS_SENSE_MAX              150
#define HRS_SENSE_MIN               80


int64_t hrs_read(void)
{
    uint32_t heart_rate;
    int32_t direction;
    bpf_fetch_local(HRS_SENSE_KEY_VALUE, &heart_rate);
    bpf_fetch_local(HRS_SENSE_KEY_DIRECTION, (uint32_t*)&direction);

    if (heart_rate == 0) {
        /* Initial setup */
        heart_rate = HRS_SENSE_MIN;
        direction = 1; /* up */
    }

    if (direction) {
        heart_rate += 2;
    }
    else {
        heart_rate -= 2;
    }

    if ((heart_rate == HRS_SENSE_MIN) || (heart_rate == HRS_SENSE_MAX)) {
        /* flip direction */
        bpf_store_local(HRS_SENSE_KEY_DIRECTION, direction ^ 1);
    }

    bpf_store_local(HRS_SENSE_KEY_VALUE, heart_rate);
    return heart_rate;
}

