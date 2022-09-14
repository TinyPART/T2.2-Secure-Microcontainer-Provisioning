#include <stddef.h>
#include <stdint.h>
#include "saul.h"

WASM_EXPORT int main(int argc, char **argv)
{
    int type = SAUL_SENSE_TEMP;
    uintptr_t device = saul_reg_find_type(type);
    return (int)(saul_reg_read(device) * 100);
}
