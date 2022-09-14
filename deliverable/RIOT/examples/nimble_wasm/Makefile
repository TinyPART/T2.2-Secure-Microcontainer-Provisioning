# name of your application
APPLICATION = nimble_heart_rate_sensor

# If no BOARD is found in the environment, use this default:
BOARD ?= nrf52840dk

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Some RIOT modules needed for this example
USEMODULE += event_timeout

# Include NimBLE
USEPKG += nimble
USEPKG += wamr
USEMODULE += nimble_svc_gap
USEMODULE += nimble_svc_gatt

# We also use the AD part of the BLE helper module
USEMODULE += bluetil_ad

# add bpf and saul parts
USEMODULE += saul
USEMODULE += saul_reg
USEMODULE += saul_default
USEMODULE += ztimer_usec

USEMODULE += ps

BLOBS += wasm/hello.wasm
BLOBS += wasm/hrs.wasm

CFLAGS += -DTHREAD_STACKSIZE_MAIN='(4 * 1024)'
CFLAGS += -DNIMBLE_HOST_STACKSIZE='(4 * 1024)'
export WAMR_CONFIG := $(abspath config.cmake)

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
