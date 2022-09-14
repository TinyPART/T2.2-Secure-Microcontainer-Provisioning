#include <stdint.h>
#include "bpf/bpfapi/helpers.h"

int temp_read(uint64_t *type)
{
    bpf_saul_reg_t *sensor;
    phydat_t measurement;

    /* Find temp sensor */
    sensor = bpf_saul_reg_find_type(*type);

    if (!sensor ||
        (bpf_saul_reg_read(sensor,
                           &measurement) < 0)) {
        return -1;
    }

    /** format */
    return measurement.val[0] * 100;

    ;
}
