#include <stddef.h>
#include <stdint.h>

#define WASM_EXPORT __attribute__((visibility("default")))

#define HRS_MIN 80
#define HRS_MAX 150

static int direction = 2;
static int value = HRS_MIN;

WASM_EXPORT int main(int argc, char **argv)
{
    if (value == HRS_MIN) {
        direction = 2;
    }
    else if (value == HRS_MAX) {
        direction = -2;
    }
    value += direction;
    return value;
}

